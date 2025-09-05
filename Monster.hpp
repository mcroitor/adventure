#pragma once

#include <string>

#include "Item.hpp"
#include "Point.hpp"

class Monster {
    Point position;

    std::string name;
    int health;
    int agility;
    int strength;

    int viewRadius;

    Item helm;
    Item armor;
    Item weapon;
    Item boots;
    Item shield;
public:
    int Hit(int damage);
    int ViewRadius() const;
    Point GetPosition() const;
};
