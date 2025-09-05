#pragma once

#include <string>

#include "Item.hpp"
#include "Point.hpp"
#include "Direction.hpp"

class Player {
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
/*
    defense = helm.defense + 
            armor.defense + 
            weapon.defense + 
            boots.defense + 
            shield.defense;
    if(defense < damage )
        health -= (damage - defense)
*/
    int Hit(int damage);
    int ViewRadius() const;
    Point GetPosition() const;
    void Move(Direction direction);
};