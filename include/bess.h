#ifndef BESS_H
#define BESS_H

#include <array>

class BESS {
private:
  std::array<double, 24> consumer_curve;
  std::array<double, 24> resulting_curve;

  int discharge_start_hour;
  int discharge_end_hour;

public:
  double get_daily_stored_energy();
  double get_monthly_stored_energy();

  void set_discharge_interval(int start_hour, int end_hour);

  void generate_resulting_curve();
  double get_power_at_hour(int hour);

  BESS(std::array<double, 24> consumer_curve);
  ~BESS();
};

#endif // BESS_H
