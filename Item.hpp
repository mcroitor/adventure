#pragma once

#include <string>

class Item {
    std::string name_;
    int defense_;
    int attack_;

public:
    Item() = default;
    Item(const std::string& name, int def, int att);
    int GetDefense() const;
    int GetAttack() const;
};

const Item NO_ITEM {"no item", 0, 0};
