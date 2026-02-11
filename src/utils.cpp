#include "utils.h"

std::pair<std::size_t, std::size_t> getComplementInterval(
    const std::pair<std::size_t, std::size_t> interval) 
{
    std::size_t new_start = (interval.second + 1) % HOURS;
    std::size_t new_end = (interval.first + HOURS - 1) % HOURS;

    return {new_start, new_end};
}