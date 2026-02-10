#ifndef BESS_H
#define BESS_H

#include <array>

class BESS {
public:
  explicit BESS(const std::array<double, 24> &consumer_curve);

  double getDailyStoredEnergy() const;
  double getMonthlyStoredEnergy() const;
  void setDischargeInterval(const int start_hour, const int end_hour);
  void generateResultingCurve();
  double getPowerAtHour(int hour) const;

private:
  std::array<double, 24> consumer_curve{};
  std::array<double, 24> resulting_curve{};

  double daily_stored_energy = 0.0;
  int discharge_start_index = 0;
  int discharge_end_index = 0;
};

#endif // BESS_H