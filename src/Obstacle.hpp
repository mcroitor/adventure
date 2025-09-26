#pragma once

#include "Point.hpp"

class Obstacle {
    int type;
    Point position;
public:
    Point GetPosition() const;
};
