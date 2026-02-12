#ifndef BESS_H
#define BESS_H

#include <array>
#include <utility>
#include <cstddef>

static constexpr std::size_t HOURS = 24;

class BESS {
public:
  explicit BESS(const std::array<double, HOURS> &consumer_curve);

  void setConsumerCurve(const std::array<double, HOURS> &consumer_curve);
  std::array<double, HOURS> getConsumerCurve() const; 

  double getDailyStoredEnergy() const;
  double getMonthlyStoredEnergy() const;

  void setDischargeInterval(std::pair<std::size_t, std::size_t> interval);
  std::pair<std::size_t, std::size_t> getDischargeInterval() const; 

  void generateResultingCurve();
  std::array<double, HOURS> getResultingCurve() const; 

  double getPowerAtHour(const std::size_t hour) const;  
  
  const double* data() const;
  const std::array<double, HOURS>& getResultingCurveRef() const; 


private:
  std::array<double, HOURS> consumer_curve{};
  std::array<double, HOURS> resulting_curve{};
  

  double daily_stored_energy = 0.0;
  double interval_consumption = 0.0;

  std::pair<std::size_t, std::size_t> discharge_interval{};
};

#endif // BESS_H