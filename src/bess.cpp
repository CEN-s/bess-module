#include "bess.h"
#include "utils.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <numeric>
#include <stdexcept>
#include <utility>

BESS::BESS(const std::array<double, HOURS> &consumer_curve)
    : consumer_curve(consumer_curve) {
  daily_stored_energy = std::accumulate(
      consumer_curve.begin(), consumer_curve.end(), 0.0,
      [](double acc, double val) { return acc + std::max(0.0, -val); });
}

std::array<double, HOURS> BESS::getResultingCurve() const { return resulting_curve; }


void BESS::setConsumerCurve(const std::array<double, HOURS> &curve) {
  consumer_curve = curve;

  daily_stored_energy = std::accumulate(
      consumer_curve.begin(), consumer_curve.end(), 0.0,
      [](double acc, double val) { return acc + std::max(0.0, -val); });
}

std::array<double, HOURS> BESS::getConsumerCurve() const {
  return consumer_curve;
}

double BESS::getDailyStoredEnergy() const { return daily_stored_energy; }

double BESS::getMonthlyStoredEnergy() const { return daily_stored_energy * 30; }

void BESS::setDischargeInterval(std::pair<std::size_t, std::size_t> input_interval) {
  std::size_t start_hour = input_interval.first;
  std::size_t end_hour = input_interval.second;

  if (start_hour < 1 || start_hour > HOURS ||
       end_hour < 1 || end_hour > HOURS) {
    throw std::invalid_argument("Input hours must be between 1 and 24");
  }
  const std::size_t start_index = static_cast<std::size_t>(start_hour - 1);
  const std::size_t end_index = static_cast<std::size_t>(end_hour - 1);

  std::pair<std::size_t, std::size_t> interval = {start_index, end_index};

  bool has_generation = false;

  forEachInterval(consumer_curve, interval,
                  [&has_generation](double val){
                    has_generation |= val < 0;
                  });

  if(has_generation){
    throw std::invalid_argument("Cannot set interval where generation occurs");
  }
  discharge_interval = interval;
}

std::pair<std::size_t, std::size_t> BESS::getDischargeInterval() const {
  return discharge_interval;
}

void BESS::generateResultingCurve() {
  resulting_curve = consumer_curve;

  double window_consumption = 0.0;

  forEachInterval(consumer_curve, discharge_interval,
                  [&window_consumption](double val){
                    window_consumption += val;
                  });
  
  double discharge_ratio =
      std::max(0.0, 1 - (daily_stored_energy / window_consumption));

  forEachInterval(consumer_curve, resulting_curve, discharge_interval,
                  [&discharge_ratio](double val){
                    return val * discharge_ratio;
                  });

  const double remaining_energy =std::max(0.0, daily_stored_energy - window_consumption);
  const double remaining_ratio = remaining_energy / daily_stored_energy;

  std::pair<std::size_t, std::size_t> complement_interval = getComplementInterval(discharge_interval);

  forEachInterval(consumer_curve, resulting_curve, complement_interval,
                  [&remaining_ratio](double val){
                    return val * remaining_ratio;
                  });

}

double BESS::getPowerAtHour(const std::size_t hour) const {
  return resulting_curve[hour - 1];
}

const double* BESS::data() const { return consumer_curve.data(); }

const double* BESS::begin() const { return resulting_curve.begin(); }
const double* BESS::end() const { return resulting_curve.end(); }
