#ifndef BESS_H
#define BESS_H

#include <array>

class BESS {
 private:
  std::array<double, 24> consumer_curve{};
  std::array<double, 24> resulting_curve{};

  int discharge_start_hour = 0;
  int discharge_end_hour = 0;

 public:
  double getDailyStoredEnergy() const;
  double getMonthlyStoredEnergy() const;

  void setDischargeInterval(int start_hour, int end_hour);

  void generateResultingCurve();
  double getPowerAtHour(int hour) const;

  explicit BESS(std::array<double, 24> consumer_curve);
};

#endif
