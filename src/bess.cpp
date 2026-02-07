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

  auto size = (end_hour - start_hour + 24) % 24 + 1;

  auto window = std::views::iota(0, size) | std::views::transform([&](int i) {
                  return consumer_curve[(start_hour - 1 + i) % 24];
                });

  bool has_generation =
      std::ranges::any_of(window, [](double val) { return val < 0; });

  if (has_generation) {
    throw std::invalid_argument(
        "Not possible to define an interval where there is generation.");
  }

  discharge_start_index = start_hour - 1;
  discharge_end_index = end_hour - 1;
}

void BESS::generateResultingCurve() {
  std::ranges::transform(consumer_curve, resulting_curve.begin(),
                         [](double val) { return std::max(0.0, val); });

  const int size = (discharge_end_index - discharge_start_index + 24) % 24 + 1;

  auto target_indices =
      std::views::iota(0, size) | std::views::transform([&](int i) {
        return (discharge_start_index + i) % 24;
      });

  auto window = std::views::iota(0, size) | std::views::transform([&](int i) {
                  return consumer_curve[(discharge_start_index + i) % 24];
                });

  double window_consumption =
      std::accumulate(window.begin(), window.end(), 0.0);

  if (window_consumption > 0.0) {
    const double stored_energy = getDailyStoredEnergy();

    std::ranges::for_each(
        target_indices, [this, window_consumption, stored_energy](int i) {
          double current_val = consumer_curve[i];
          double energy_to_dispatch =
              stored_energy * (current_val / window_consumption);

          resulting_curve[i] = std::max(0.0, current_val - energy_to_dispatch);
        });
  }
}

double BESS::getPowerAtHour(int hour) const {
  return resulting_curve[hour - 1];
}