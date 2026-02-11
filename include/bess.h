#ifndef BESS_H
#define BESS_H

#include <array>
#include <utility>
#include <cstddef>

static constexpr int HOURS = 24;

class BESS {
public:
  explicit BESS(const std::array<double, HOURS> &consumer_curve);

  void setConsumerCurve(const std::array<double, HOURS> &consumer_curve);
  double getDailyStoredEnergy() const;
  double getMonthlyStoredEnergy() const;
  void setDischargeInterval(const int start_hour, const int end_hour);
  void generateResultingCurve();
  double getResultingCurve() const; 
  double getPowerAtHour(const int hour) const;  

private:
  std::array<double, HOURS> consumer_curve{};
  std::array<double, HOURS> resulting_curve{};

  double daily_stored_energy = 0.0;
  double interval_consumption = 0.0;

  std::pair<std::size_t, std::size_t> discharge_interval{};
};

#endif // BESS_H