#include "Point.hpp"

bool operator==(const Point& lhs, const Point& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

std::ostream& operator<<(std::ostream& os, const Point& point){
    os << point.x << " " << point.y;
    return os;
}

std::istream& operator>>(std::istream& is, Point& point){
    is >> point.x >> point.y;
    return is;
}