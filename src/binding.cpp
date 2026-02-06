#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "bess.h"

namespace py = pybind11;

PYBIND11_MODULE(bess_module, m) {
  m.doc() =
      "Daimon BESS Simulation Library - Ferramenta para análise de "
      "armazenamento de energia.";
  m.attr("__version__") = "1.0.0";

  py::class_<BESS>(
      m, "BESS",
      "Classe para simulação de Battery Energy Storage Systems (BESS)")
      .def(py::init<std::array<double, 24>>(), py::arg("consumer_curve"),
           "Inicializa o BESS com a curva de carga de 24 horas (kW).")

      .def_property_readonly("daily_stored_energy", &BESS::getDailyStoredEnergy,
                             "Retorna a energia diária total armazenada em kWh "
                             "(módulo dos valores negativos).")

      .def_property_readonly("monthly_stored_energy",
                             &BESS::getMonthlyStoredEnergy,
                             "Retorna a estimativa de energia armazenada "
                             "mensalmente (considerando 30 dias).")

      .def("set_discharge_interval", &BESS::setDischargeInterval,
           py::arg("start_hour"), py::arg("end_hour"),
           "Define a janela de tempo (h) para a descarga da bateria. Não "
           "permite intervalos com geração.")

      .def("generate_resulting_curve", &BESS::generateResultingCurve,
           "Gera a nova estrutura de dados com a energia distribuída no "
           "intervalo definido de consumo.")

      .def("get_power_at_hour", &BESS::getPowerAtHour, py::arg("hour"),
           "Retorna o valor da potência (kW) na curva resultante para um "
           "instante 't'.");
}