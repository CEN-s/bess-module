#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <pybind11/operators.h>
#include <utility>

#include "bess.h"

namespace py = pybind11;

PYBIND11_MODULE(_bess, m) {
    m.doc() = R"pbdoc(
_bess
=====

Backend C++ de alta performance para simulação de sistemas BESS
(Battery Energy Storage System).

O módulo expõe a classe :class:`BESS`, que simula armazenamento e
despacho de energia ao longo de uma curva de 24 horas.

Características:
- Compatível com NumPy (buffer protocol)
- Iterável como sequência Python
- Suporte a pickle
- Execução thread-safe
)pbdoc";

    // constante
    m.attr("HOURS") = py::int_(24);

    py::class_<BESS>(m, "BESS", py::buffer_protocol(), R"pbdoc(
BESS(consumer_curve)

Simulador de sistema de armazenamento em bateria (BESS).

A classe recebe uma curva de 24 horas contendo consumo e/ou geração
e simula o armazenamento de excedente e posterior descarga em um
intervalo configurável.

Parâmetros
----------
consumer_curve : Sequence[float]
    Curva horária com 24 valores.

    Convenção:
    - valores positivos → consumo
    - valores negativos → geração (ex: solar)

Notas
-----
- O objeto é iterável e se comporta como uma sequência de 24 valores
  representando a curva resultante após a simulação.
- Compatível com `numpy.array(bess)`.
)pbdoc")

        // initialization
        .def(py::init<const std::array<double, 24> &>(),
             py::arg("consumer_curve"),
             R"pbdoc(
Inicializa o sistema BESS com uma curva horária de 24 pontos.

Parâmetros
----------
consumer_curve : array-like de 24 floats
    Curva de consumo/geração em kW.

Lança
-----
ValueError
    Se a curva não tiver exatamente 24 valores.
)pbdoc")

        // buffer protocol
        .def_buffer([](BESS &self) -> py::buffer_info {
            return py::buffer_info(
                const_cast<double*>(self.data()), // data pointer
                sizeof(double), // item size
                py::format_descriptor<double>::format(), // format
                1, // ndim
                {24}, // shape
                {sizeof(double)}, // strides
                true
            );
        })

        // properties
        .def_property(
            "consumer_curve",
            &BESS::getConsumerCurve,
            &BESS::setConsumerCurve,
            R"pbdoc(
Curva original de consumo/geração.

Tipo
----
list[float] (tamanho 24)

Observações
-----------
- Valores negativos representam geração.
- Alterar a curva invalida a simulação atual.
)pbdoc")

        .def_property_readonly(
            "daily_stored_energy",
            &BESS::getDailyStoredEnergy,
            R"pbdoc(
Energia total armazenada no dia.

Retorna
-------
float
    Energia em kWh armazenada ao longo do dia.
)pbdoc")

        .def_property_readonly(
            "monthly_stored_energy",
            &BESS::getMonthlyStoredEnergy,
            R"pbdoc(
Projeção mensal de energia armazenada.

Retorna
-------
float
    Energia mensal estimada em kWh.
)pbdoc")

        .def_property_readonly(
            "resulting_curve",
            &BESS::getResultingCurve,
            R"pbdoc(
Curva resultante após a simulação do BESS.

Retorna
-------
list[float]
    Curva de 24 valores representando a carga final após
    armazenamento e descarga.
)pbdoc")

        .def_property(
            "discharge_interval",
            [](const BESS &self) {
                auto interval = self.getDischargeInterval();
                return std::make_pair(
                    static_cast<int>(interval.first + 1),
                    static_cast<int>(interval.second + 1)
                );
            },
            [](BESS &self, std::pair<int, int> interval) {
                auto converted_interval = std::make_pair(
                    static_cast<std::size_t>(interval.first),
                    static_cast<std::size_t>(interval.second)
                );
                self.setDischargeInterval(converted_interval);
            },
            R"pbdoc(
Intervalo de descarga da bateria.

Formato
-------
tuple[int, int]  (start, end)

Escala
------
1 a 24 (inclusive)

Exemplo
-------
>>> bess.discharge_interval = (18, 22)

Notas
-----
- O intervalo é inclusivo.
- Não pode conter horas com geração.
- A validação ocorre no backend C++.
)pbdoc")

        // methods
        .def(
            "generate_resulting_curve",
            &BESS::generateResultingCurve,
            py::call_guard<py::gil_scoped_release>(),
            R"pbdoc(
Executa a simulação do BESS.

Este método:
- armazena excedentes
- aplica descarga no intervalo definido
- gera a curva resultante

Thread-safe e libera o GIL durante a execução.
)pbdoc")

        .def(
            "get_power_at_hour",
            &BESS::getPowerAtHour,
            py::arg("hour"),
            R"pbdoc(
Retorna a potência na hora especificada.

Parâmetros
----------
hour : int
    Hora no intervalo 1–24.

Retorna
-------
float
    Potência na hora especificada após simulação.
)pbdoc")

        // iter
        .def("__iter__", [](const BESS &s) {
            auto& curve = s.getResultingCurveRef();
            return py::make_iterator(curve.begin(), curve.end());
        }, py::keep_alive<0, 1>(),
        R"pbdoc(Itera sobre a curva resultante.)pbdoc")

        .def("__len__", [](const BESS &) { return 24; },
        R"pbdoc(Retorna 24.)pbdoc")

        .def("__getitem__", [](const BESS &s, int i) {
            if (i < 0) i += 24;
            if (i < 0 || i >= 24) throw py::index_error();
            return s.getResultingCurve()[i];
        },
        R"pbdoc(
Acesso indexado à curva resultante.

Suporta índices negativos.
)pbdoc")

        // pickle
        .def(py::pickle(
            [](const BESS &self) {
                return py::make_tuple(
                    self.getConsumerCurve(),
                    self.getDischargeInterval()
                );
            },
            [](py::tuple t) {
                if (t.size() != 2)
                    throw std::runtime_error("Invalid state for BESS");

                BESS p(t[0].cast<std::array<double, 24>>());

                std::pair<size_t, size_t> raw_interval =
                    t[1].cast<std::pair<size_t, size_t>>();

                auto adjusted_interval =
                    std::make_pair(raw_interval.first + 1,
                                   raw_interval.second + 1);

                p.setDischargeInterval(adjusted_interval);
                p.generateResultingCurve();

                return p;
            }
        ));
}
