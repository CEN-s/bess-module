#include "bess.h"

#include <array>
#include <stdexcept>

BESS::BESS(std::array<double, 24> consumer_curve)
    : consumer_curve(consumer_curve) {}

double BESS::getDailyStoredEnergy() const {
  double res = 0.0;
  for (const auto& n : consumer_curve) {
    if (n < 0) {
      res += n;
    }
  }
  return std::abs(res);
}

double BESS::getMonthlyStoredEnergy() const {
  return getDailyStoredEnergy() * 30;
}

void BESS::setDischargeInterval(const int start_hour, const int end_hour) {
  if (start_hour < 1 || start_hour > 24 || end_hour < 1 || end_hour > 24) {
    throw std::invalid_argument("Hour must be between 1 and 24.");
  }

  int i = start_hour - 1;
  while (i != end_hour) {
    if (consumer_curve[i] < 0) {
      throw std::invalid_argument(
          "Not possible to define an interval where there is generation.");
    }
    i = (i + 1) % 24;
  }

  discharge_start_hour = start_hour - 1;
  discharge_end_hour = end_hour - 1;
}

void BESS::generateResultingCurve() {
  for (int i = 0; i < 24; ++i) {
    if (consumer_curve[i] < 0) {
      resulting_curve[i] = 0.0;
    } else {
      resulting_curve[i] = consumer_curve[i];
    }
  }

  double total_consumption_in_window = 0.0;
  int i = discharge_start_hour;
  while (i != std::max(discharge_end_hour + 1, 23)) {
    if (consumer_curve[i] > 0) {
      total_consumption_in_window += consumer_curve[i];
    }
    i = (i + 1) % 24;
  }

  const double stored_energy = getDailyStoredEnergy();

  if (total_consumption_in_window > 0) {
    i = discharge_start_hour;
    while (i != std::max(discharge_end_hour + 1, 23)) {
      resulting_curve[i] -=
          stored_energy * (consumer_curve[i] / total_consumption_in_window);
      if (resulting_curve[i] < 0) {
        resulting_curve[i] = 0.0;
      }

      i = (i + 1) % 24;
    }
  }
}

double BESS::getPowerAtHour(int hour) const { return resulting_curve[hour]; }