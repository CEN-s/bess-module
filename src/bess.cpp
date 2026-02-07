#include "bess.h"

#include <algorithm>
#include <array>
#include <numeric>
#include <ranges>
#include <stdexcept>

BESS::BESS(std::array<double, 24> consumer_curve)
    : consumer_curve(consumer_curve) {}

double BESS::getDailyStoredEnergy() const {
  return std::accumulate(
      consumer_curve.begin(), consumer_curve.end(), 0.0,
      [](double acc, double val) { return acc + std::max(0.0, -val); });
}

double BESS::getMonthlyStoredEnergy() const {
  return getDailyStoredEnergy() * 30;
}

void BESS::setDischargeInterval(const int start_hour, const int end_hour) {
  if (start_hour < 1 || start_hour > 24 || end_hour < 1 || end_hour > 24) {
    throw std::invalid_argument("Hour must be between 1 and 24.");
  }

  int start_idx = start_hour - 1;
  int end_idx = end_hour - 1;

  bool has_generation = false;
  if (start_hour > end_hour) {
    has_generation =
        std::any_of(consumer_curve.begin() + start_idx, consumer_curve.end(),
                    [](double val) { return val < 0; }) ||
        std::any_of(consumer_curve.begin(), consumer_curve.begin() + end_idx,
                    [](double val) { return val < 0; });
  } else {
    has_generation = std::any_of(consumer_curve.begin() + start_idx,
                                 consumer_curve.begin() + end_idx,
                                 [](double val) { return val < 0; });
  }

  if (has_generation) {
    throw std::invalid_argument(
        "Not possible to define an interval where there is generation.");
  }

  discharge_start_index = start_idx;
  discharge_end_index = end_idx;
}

void BESS::generateResultingCurve() {
  resulting_curve = consumer_curve;

  double stored_energy = getDailyStoredEnergy();
  double window_consumption = 0.0;

  if (discharge_start_index > discharge_end_index) {
    window_consumption =
        std::accumulate(consumer_curve.begin() + discharge_start_index,
                        consumer_curve.end(), 0.0,
                        [](double acc, double val) { return acc + val; }) +
        std::accumulate(consumer_curve.begin(),
                        consumer_curve.begin() + discharge_end_index, 0.0,
                        [](double acc, double val) { return acc + val; });
    double discharge_ratio = 1 - (stored_energy / window_consumption);
    std::transform(consumer_curve.begin() + discharge_start_index,
                   consumer_curve.end(),
                   resulting_curve.begin() + discharge_start_index,
                   [discharge_ratio](double val) {
                     return std::max(0.0, val * discharge_ratio);
                   });
    std::transform(consumer_curve.begin(),
                   consumer_curve.begin() + discharge_end_index,
                   resulting_curve.begin(), [discharge_ratio](double val) {
                     return std::max(0.0, val * discharge_ratio);
                   });
  } else {
    window_consumption =
        std::accumulate(consumer_curve.begin() + discharge_start_index,
                        consumer_curve.begin() + discharge_end_index, 0.0,
                        [](double acc, double val) { return acc + val; });
    double discharge_ratio = 1 - (stored_energy / window_consumption);
    std::transform(consumer_curve.begin() + discharge_start_index,
                   consumer_curve.begin() + discharge_end_index,
                   resulting_curve.begin() + discharge_start_index,
                   [discharge_ratio](double val) {
                     return std::max(0.0, val * discharge_ratio);
                   });
  }
}

double BESS::getPowerAtHour(int hour) const {
  return resulting_curve[hour - 1];
}