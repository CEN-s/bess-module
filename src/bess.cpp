#include "bess.h"
#include "utils.h"

#include <algorithm>
#include <array>
#include <numeric>
#include <stdexcept>
#include <utility>

BESS::BESS(const std::array<double, HOURS> &consumer_curve)
    : consumer_curve(consumer_curve) {
  daily_stored_energy = std::accumulate(
      consumer_curve.begin(), consumer_curve.end(), 0.0,
      [](double acc, double val) { return acc + std::max(0.0, -val); });
}

void BESS::setConsumerCurve(const std::array<double, HOURS> &curve) {
  consumer_curve = curve;

  daily_stored_energy = std::accumulate(
      consumer_curve.begin(), consumer_curve.end(), 0.0,
      [](double acc, double val) { return acc + std::max(0.0, -val); });
}

double BESS::getDailyStoredEnergy() const { return daily_stored_energy; }

double BESS::getMonthlyStoredEnergy() const { return daily_stored_energy * 30; }

void BESS::setDischargeInterval(const int start_hour, const int end_hour) {
  if (start_hour < 1 || start_hour > HOURS ||
       end_hour < 1 || end_hour > HOURS) {
    throw std::invalid_argument("Hour must be between 1 and 24.");
  }

  const int start_index = start_hour - 1;
  const int end_index = end_hour - 1;

  std::pair<std::size_t, std::size_t> interval = {start_index, end_index};

  bool has_generation = false;

  forEachInterval(consumer_curve, interval,
                  [&has_generation](double val){
                    has_generation |= val < 0;
                  });

  if(has_generation){
    throw std::invalid_argument("Cannot set interval where generation occurs.");
  }

  discharge_interval = interval;
}

void BESS::generateResultingCurve() {
  resulting_curve = consumer_curve;

  double window_consumption = 0.0;

  forEachInterval(consumer_curve, discharge_interval,
                  [&window_consumption](double val){
                    window_consumption += val;
                  });
  
  double discharge_ratio = 1 - (daily_stored_energy / window_consumption);

  forEachInterval(consumer_curve, resulting_curve, discharge_interval,
                  [&discharge_ratio](double val){
                    return val * discharge_ratio;
                  });

  const double remaining_energy =std::max(0.0, daily_stored_energy - window_consumption);
  const double remaining_ratio = 1 - (remaining_energy / daily_stored_energy);

  std::pair<std::size_t, std::size_t> complement_interval = getComplementInterval(discharge_interval);

  forEachInterval(consumer_curve, resulting_curve, complement_interval,
                  [&remaining_ratio](double val){
                    return val * remaining_ratio;
                  });

}

double BESS::getResultingCurve() const {
  return resulting_curve;
}

double BESS::getPowerAtHour(const int hour) const {
  return resulting_curve[hour - 1];
}
