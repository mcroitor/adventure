#include "Obstacle.hpp"

Obstacle::Obstacle(): type(0), position({0, 0}) {}

Obstacle::Obstacle(int t, const Point& pos) : type(t), position(pos) {}

Obstacle::Obstacle(const Obstacle& other) : type(other.GetType()), position(other.GetPosition()) {}

Point Obstacle::GetPosition() const{
    return position;
}

int Obstacle::GetType() const {
    return type;
}

Obstacle& Obstacle::operator=(const Obstacle& other){
    type = other.GetType();
    position = other.GetPosition();
    return *this;
}

bool Obstacle::operator==(const Obstacle& other) const {
    return type == other.type && position == other.GetPosition();
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