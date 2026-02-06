#include "bess.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CalculateStoredEnergy", "[energy]") {
  std::array<double, 24> curve;
  curve.fill(10.0);

  curve[10] = -5.0;
  curve[11] = -10.0;
  curve[12] = -5.0;

  BESS bess(curve);

  double expected_daily = 20.0;

  REQUIRE(bess.get_daily_stored_energy() == expected_daily);
  REQUIRE(bess.get_monthly_stored_energy() == expected_daily * 30.0);
}

TEST_CASE("ValidateDischargeInterval", "[interval]") {
  std::array<double, 24> curve;
  curve.fill(10.0);
  curve[12] = -5.0;

  BESS bess(curve);

  SECTION("Valid simple interval") {
    REQUIRE_NOTHROW(bess.set_discharge_interval(18, 22));
  }

  SECTION("Valid circular interval") {
    REQUIRE_NOTHROW(bess.set_discharge_interval(20, 5));
  }

  SECTION("Invalid: overlapping with generation") {
    REQUIRE_THROWS_AS(bess.set_discharge_interval(10, 14),
                      std::invalid_argument);
  }

  SECTION("Invalid: circular overlapping with generation") {
    REQUIRE_THROWS_AS(bess.set_discharge_interval(10, 5),
                      std::invalid_argument);
  }

  SECTION("Invalid: Out of bounds") {
    REQUIRE_THROWS(bess.set_discharge_interval(-1, 10));
    REQUIRE_THROWS(bess.set_discharge_interval(0, 24));
  }
}

TEST_CASE("SimulateSimpleDischarge", "[simulation]") {
  std::array<double, 24> curve;
  curve.fill(5.0);

  curve[10] = -10.0;
  curve[11] = -10.0;

  BESS bess(curve);

  bess.set_discharge_interval(18, 20);
  bess.generate_resulting_curve();

  REQUIRE(bess.get_power_at_hour(10) == 0.0);
  REQUIRE(bess.get_power_at_hour(11) == 0.0);

  REQUIRE(bess.get_power_at_hour(18) == 0.0);
  REQUIRE(bess.get_power_at_hour(19) == 0.0);

  REQUIRE(bess.get_power_at_hour(0) == 5.0);
}

TEST_CASE("SimulateProportionalDischarge", "[simulation]") {
  std::array<double, 24> curve;
  curve.fill(10.0);

  curve[12] = -10.0;

  BESS bess(curve);

  bess.set_discharge_interval(18, 20);
  bess.generate_resulting_curve();

  REQUIRE(bess.get_power_at_hour(18) == 5.0);
  REQUIRE(bess.get_power_at_hour(19) == 5.0);
}

TEST_CASE("SimulateCircularDischarge", "[simulation][circular]") {
  std::array<double, 24> curve;
  curve.fill(10.0);
  curve[12] = -20.0;

  BESS bess(curve);

  bess.set_discharge_interval(22, 2);
  bess.generate_resulting_curve();

  REQUIRE(bess.get_power_at_hour(22) == 5.0);
  REQUIRE(bess.get_power_at_hour(23) == 5.0);
  REQUIRE(bess.get_power_at_hour(0) == 5.0);
  REQUIRE(bess.get_power_at_hour(1) == 5.0);

  REQUIRE(bess.get_power_at_hour(21) == 10.0);
  REQUIRE(bess.get_power_at_hour(2) == 10.0);
}
