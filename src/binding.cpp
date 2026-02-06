#include "bess.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

PYBIND11_MODULE(bess, m) {
  py::class_<BESS>(m, "BESS")
      .def(py::init<std::array<double, 24>>(), py::arg("consumer_curve"))
      .def("get_daily_stored_energy", &BESS::get_daily_stored_energy)
      .def("get_monthly_stored_energy", &BESS::get_monthly_stored_energy)
      .def("set_discharge_interval", &BESS::set_discharge_interval,
           py::arg("start_hour"), py::arg("end_hour"))
      .def("generate_resulting_curve", &BESS::generate_resulting_curve)
      .def("get_power_at_hour", &BESS::get_power_at_hour, py::arg("hour"));
}