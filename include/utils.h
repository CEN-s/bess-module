#include "bess.h"
#include <utility>
#include <cstddef>

template <typename Func>
void forEachInterval(const std::array<double, HOURS>& container, 
                     const std::pair<std::size_t, std::size_t> interval,
                     const Func func) {

    auto start = container.begin() + interval.first;
    auto stop = container.begin() + interval.second + 1;

    if(interval.first <= interval.second){
        for(auto it = start; it < stop; ++it) {
            func(*it);
        }
    } else {
        for(auto it = start; it < container.end(); ++it) {
            func(*it);
        }
        for(auto it = container.begin(); it < stop; ++it) {
            func(*it);
        }
    }
}

template <typename Func>
void forEachInterval(const std::array<double, HOURS>& input_container,
                     std::array<double, HOURS>& output_container,
                     const std::pair<std::size_t, std::size_t> interval,
                     const Func func) {
    if (interval.first <= interval.second) {
        for (std::size_t i = interval.first; i <= interval.second; ++i) {
            output_container[i] = func(input_container[i]);
        }
    } else {
        for (std::size_t i = interval.first; i < HOURS; ++i) {
            output_container[i] = func(input_container[i]);
        }
        for (std::size_t i = 0; i <= interval.second; ++i) {
            output_container[i] = func(input_container[i]);
        }
    }
}

std::pair<std::size_t, std::size_t> getComplementInterval(
    const std::pair<std::size_t, std::size_t> interval);
