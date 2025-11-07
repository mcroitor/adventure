#include "Monster.hpp"

int Monster::Hit(int damage) {
    return 0;
}

int Monster::ViewRadius() const {
    return 1;
}

Point Monster::GetPosition() const {
    return position;
}