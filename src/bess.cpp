#include "bess.h"

#include <algorithm>
#include <array>
#include <numeric>
#include <stdexcept>

BESS::BESS(const std::array<double, 24> &consumer_curve)
    : consumer_curve(consumer_curve) {
  daily_stored_energy = std::accumulate(
      consumer_curve.begin(), consumer_curve.end(), 0.0,
      [](double acc, double val) { return acc + std::max(0.0, -val); });
}

double BESS::getDailyStoredEnergy() const { return daily_stored_energy; }

double BESS::getMonthlyStoredEnergy() const { return daily_stored_energy * 30; }

void BESS::setDischargeInterval(const int start_hour, const int end_hour) {
  if (start_hour < 1 || start_hour > 24 || end_hour < 1 || end_hour > 24) {
    throw std::invalid_argument("Hour must be between 1 and 24.");
  }

  int start_index = start_hour - 1;
  int end_index = end_hour - 1;

  bool has_generation = false;

  if (start_hour > end_hour) {
    has_generation =
        std::any_of(consumer_curve.begin() + start_index, consumer_curve.end(),
                    [](double val) { return val < 0; }) ||
        std::any_of(consumer_curve.begin(),
                    consumer_curve.begin() + end_index + 1,
                    [](double val) { return val < 0; });
  } else {
    has_generation = std::any_of(consumer_curve.begin() + start_index,
                                 consumer_curve.begin() + end_index + 1,
                                 [](double val) { return val < 0; });
  }

  if (has_generation) {
    throw std::invalid_argument(
        "Not possible to define an interval where there is generation.");
  }

  discharge_start_index = start_index;
  discharge_end_index = end_index;
}

void BESS::generateResultingCurve() {
  resulting_curve = consumer_curve;

  double window_consumption = 0.0;

  if (discharge_start_index > discharge_end_index) {
    window_consumption =
        std::accumulate(consumer_curve.begin() + discharge_start_index,
                        consumer_curve.end(), 0.0,
                        [](double acc, double val) { return acc + val; }) +
        std::accumulate(consumer_curve.begin(),
                        consumer_curve.begin() + discharge_end_index + 1, 0.0,
                        [](double acc, double val) { return acc + val; });
    double discharge_ratio = 1 - (daily_stored_energy / window_consumption);
    std::transform(consumer_curve.begin() + discharge_start_index,
                   consumer_curve.end(),
                   resulting_curve.begin() + discharge_start_index,
                   [discharge_ratio](double val) {
                     return std::max(0.0, val * discharge_ratio);
                   });
    std::transform(consumer_curve.begin(),
                   consumer_curve.begin() + discharge_end_index + 1,
                   resulting_curve.begin(), [discharge_ratio](double val) {
                     return std::max(0.0, val * discharge_ratio);
                   });
  } else {
    window_consumption = std::accumulate(consumer_curve.begin() + discharge_start_index,
                                         consumer_curve.begin() + discharge_end_index + 1, 0.0,
                                         [](double acc, double val) { return acc + val; });

    double discharge_ratio = 1 - (daily_stored_energy / window_consumption);
    
    std::transform(consumer_curve.begin() + discharge_start_index,
                   consumer_curve.begin() + discharge_end_index + 1,
                   resulting_curve.begin() + discharge_start_index,
                   [discharge_ratio](double val) {
                     return std::max(0.0, val * discharge_ratio);
                   });
  }
  double remaining_energy = daily_stored_energy - window_consumption;
  double remaining_ratio = 1 - (remaining_energy / daily_stored_energy);

  if (remaining_energy > 0) {
    if (discharge_start_index > discharge_end_index) {
      std::transform(
          consumer_curve.begin() + discharge_end_index + 1,
          consumer_curve.begin() + discharge_start_index,
          resulting_curve.begin() + discharge_end_index + 1,
          [remaining_ratio](double val) { return val * remaining_ratio; });
    } else {
      std::transform(
          consumer_curve.begin() + discharge_end_index + 1,
          consumer_curve.end(),
          resulting_curve.begin() + discharge_end_index + 1,
          [remaining_ratio](double val) { return val * remaining_ratio; });
      std::transform(consumer_curve.begin(),
                     consumer_curve.begin() + discharge_start_index,
                     resulting_curve.begin(), [remaining_ratio](double val) {
                       return val * remaining_ratio;
                     });
    }
  } else {
    if (discharge_start_index > discharge_end_index) {
      std::fill(resulting_curve.begin() + discharge_end_index + 1,
                resulting_curve.begin() + discharge_start_index, 0.0);
    } else {
      std::fill(resulting_curve.begin() + discharge_end_index + 1,
                resulting_curve.end(), 0.0);
      std::fill(resulting_curve.begin(),
                resulting_curve.begin() + discharge_start_index, 0.0);
    }
  }
}

double BESS::getPowerAtHour(int hour) const {
  return resulting_curve[hour - 1];
}
