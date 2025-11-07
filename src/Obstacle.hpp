#pragma once

#include <iostream>

#include "Point.hpp"

class Obstacle {
    int type;
    Point position;
public:
    Obstacle();
    Obstacle(int t, const Point& pos);
    Obstacle(const Obstacle& other);
    
    Obstacle& operator=(const Obstacle& other);
    // a == b => a.operator==(b)
    bool operator==(const Obstacle& other) const;

    Point GetPosition() const;
    int GetType() const;
};

std::ostream& operator<<(std::ostream& os, const Obstacle& obstacle);
std::istream& operator>>(std::istream& is, Obstacle& obstacle);