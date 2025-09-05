#pragma once

#include <string>

class Item {
    std::string name;
    int defense;
    int attack;

public:
    int getDefense() const;
    int getAttack() const;
};
