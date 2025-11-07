#pragma once

#include <iostream>

struct Point {
    int x;
    int y;
};

bool operator==(const Point& lhs, const Point& rhs);

std::ostream& operator<<(std::ostream& os, const Point& point);
std::istream& operator>>(std::istream& is, Point& point);
