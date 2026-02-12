#include "bess.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <array>
#include <numeric>

std::array<double, 24> createCurve(double value) {
    std::array<double, 24> curve;
    curve.fill(value);
    return curve;
}

TEST_CASE("BESS Initialization and Energy Calculation", "[BESS][init]") {
    auto curve = createCurve(10.0);
    curve[0] = -10.0; 
    curve[1] = -5.0;  

    BESS bess(curve);

    SECTION("Calculates daily stored energy correctly from negative values") {
        REQUIRE(bess.getDailyStoredEnergy() == Catch::Approx(15.0));
    }

    SECTION("Calculates monthly stored energy correctly") {
        REQUIRE(bess.getMonthlyStoredEnergy() == Catch::Approx(15.0 * 30.0));
    }
}

TEST_CASE("BESS Input Validation", "[BESS][validation]") {
    auto curve = createCurve(10.0);
    curve[5] = -10.0; 
    BESS bess(curve);

    SECTION("Throws exception for invalid hour ranges") {
        CHECK_THROWS_WITH(bess.setDischargeInterval(std::make_pair(1, 25)), Catch::Matchers::Equals("Input hours must be between 1 and 24"));
        CHECK_THROWS_WITH(bess.setDischargeInterval(std::make_pair(25, 1)), Catch::Matchers::Equals("Input hours must be between 1 and 24"));
        CHECK_THROWS_WITH(bess.setDischargeInterval(std::make_pair(0, 24)), Catch::Matchers::Equals("Input hours must be between 1 and 24"));
        CHECK_THROWS_WITH(bess.setDischargeInterval(std::make_pair(24, 0)), Catch::Matchers::Equals("Input hours must be between 1 and 24"));
    }

    SECTION("Throws exception if discharge interval overlaps with generation instant") {
        CHECK_THROWS_WITH(bess.setDischargeInterval(std::make_pair(5, 7)), Catch::Matchers::Equals("Cannot set interval where generation occurs"));
    }
}

TEST_CASE("BESS Discharge Logic", "[BESS][logic]") {
    auto curve = createCurve(0.0);
    curve[0] = -20.0;

    curve[10] = 10.0; 
    curve[11] = 10.0;
    curve[12] = 10.0;
    
    BESS bess(curve);

    SECTION("Scenario A: Stored energy exactly matches window consumption") {
        bess.setDischargeInterval(std::make_pair(11, 12));
        bess.generateResultingCurve();

        CHECK(bess.getPowerAtHour(11) == Catch::Approx(0.0));
        CHECK(bess.getPowerAtHour(12) == Catch::Approx(0.0));
        CHECK(bess.getPowerAtHour(13) == Catch::Approx(10.0));
    }

    SECTION("Scenario B: Stored energy is LESS than window consumption") {
        bess.setDischargeInterval(std::make_pair(11, 13));
        bess.generateResultingCurve();

        double original = 10.0;
        double expected = original * (1.0 - (20.0 / 30.0));

        CHECK(bess.getPowerAtHour(11) == Catch::Approx(expected));
        CHECK(bess.getPowerAtHour(12) == Catch::Approx(expected)); 
        CHECK(bess.getPowerAtHour(13) == Catch::Approx(expected));
        CHECK(bess.getPowerAtHour(14) == Catch::Approx(0.0));
    }

    SECTION("Scenario C: Circular Interval (Midnight Crossing)") {
        auto circular_curve = createCurve(10.0);
        circular_curve[5] = -50.0;
        
        BESS bess_circ(circular_curve);
        REQUIRE_NOTHROW(bess_circ.setDischargeInterval(std::make_pair(23, 2)));
        
        bess_circ.generateResultingCurve();
        
        CHECK(bess_circ.getPowerAtHour(23) == Catch::Approx(0.0));
        CHECK(bess_circ.getPowerAtHour(24) == Catch::Approx(0.0));
        CHECK(bess_circ.getPowerAtHour(1) == Catch::Approx(0.0));
        CHECK(bess_circ.getPowerAtHour(2) == Catch::Approx(0.0));
        
        CHECK(bess_circ.getPowerAtHour(10) == Catch::Approx(8.0)); 
    }
}