#pragma once

#include <string>

#include "Point.hpp"

enum class Direction {
    NORTH,
    SOUTH,
    EAST,
    WEST,
    NORTH_EAST,
    NORTH_WEST,
    SOUTH_EAST,
    SOUTH_WEST,
};

std::string to_string(Direction direction);

Point DirectionToDelta(Direction direction);