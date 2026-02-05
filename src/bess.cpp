#include "bess.h"
#include <stdexcept>

BESS::BESS(std::array<double, 24> consumer_curve)
    : consumer_curve(consumer_curve) {}

BESS::~BESS() {}

double BESS::get_daily_stored_energy() {
  double res = 0.0;
  for (auto &n : consumer_curve)
    if (n < 0)
      res += n;
  return -res;
}

double BESS::get_monthly_stored_energy() {
  return get_daily_stored_energy() * 30;
}

void BESS::set_discharge_interval(int start_hour, int end_hour) {
  if (start_hour < 0 || start_hour > 23 || end_hour < 0 || end_hour > 23) {
    throw std::invalid_argument("Hour must be between 0 and 23.");
  }

  int i = start_hour;
  while (i != end_hour) {
    if (consumer_curve[i] < 0) {
      throw std::invalid_argument(
          "Not possible to define interval where there is generation.");
    }
    i = (i + 1) % 24;
  }

  discharge_start_hour = start_hour;
  discharge_end_hour = end_hour;
}

void BESS::generate_resulting_curve() {
  for (int i = 0; i < 24; ++i) {
    if (consumer_curve[i] < 0) {
      resulting_curve[i] = 0.0;
    } else {
      resulting_curve[i] = consumer_curve[i];
    }
  }

  double total_consumption_in_window = 0.0;
  int i = discharge_start_hour;
  while (i != discharge_end_hour) {
    if (consumer_curve[i] > 0) {
      total_consumption_in_window += consumer_curve[i];
    }
    i = (i + 1) % 24;
  }

  double stored_energy = get_daily_stored_energy();

  if (total_consumption_in_window > 0) {
    i = discharge_start_hour;
    while (i != discharge_end_hour) {
      double weight = consumer_curve[i] / total_consumption_in_window;
      double injection = stored_energy * weight;

      resulting_curve[i] -= injection;
      if (resulting_curve[i] < 0) {
        resulting_curve[i] = 0.0;
      }

      i = (i + 1) % 24;
    }
  }
}

double BESS::get_power_at_hour(int hour) { return resulting_curve[hour]; }