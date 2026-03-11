#include "Direction.hpp"

std::string to_string(Direction direction) {
    switch (direction) {
        case Direction::NORTH: return "NORTH";
        case Direction::SOUTH: return "SOUTH";
        case Direction::EAST: return "EAST";
        case Direction::WEST: return "WEST";
        case Direction::NORTH_EAST: return "NORTH_EAST";
        case Direction::NORTH_WEST: return "NORTH_WEST";
        case Direction::SOUTH_EAST: return "SOUTH_EAST";
        case Direction::SOUTH_WEST: return "SOUTH_WEST";
        default: return "UNKNOWN_DIRECTION";
    }
}

Point DirectionToDelta(Direction direction) {
    switch (direction) {
        case Direction::NORTH: return {0, -1};
        case Direction::SOUTH: return {0, 1};
        case Direction::EAST: return {1, 0};
        case Direction::WEST: return {-1, 0};
        case Direction::NORTH_EAST: return {1, -1};
        case Direction::NORTH_WEST: return {-1, -1};
        case Direction::SOUTH_EAST: return {1, 1};
        case Direction::SOUTH_WEST: return {-1, 1};
        default: return {0, 0};
    }
}