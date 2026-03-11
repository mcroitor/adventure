#include "Obstacle.hpp"

bool Obstacle::operator==(const Obstacle& other) const {
    return type == other.GetType() && position.x == other.GetPosition().x && position.y == other.GetPosition().y;
}

std::ostream& operator<<(std::ostream& os, const Obstacle& obstacle) {
    os << obstacle.GetType() << ' ' << obstacle.GetPosition();
    return os;
}

std::istream& operator>>(std::istream& is, Obstacle& obstacle) {
    int x, y;
    int type;
    is >> type >> x >> y;
    obstacle = Obstacle(type, Point{x, y});
    return is;
}