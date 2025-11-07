#pragma once

#include <string>
#include <iostream>

class Item {
    std::string name_;
    int defense_;
    int attack_;

public:
    Item() = default;
    Item(const std::string& name, int def, int att);
    Item(const Item& other) = default;

    Item& operator=(const Item& other) = default;
    bool operator==(const Item& other) const;

    std::string GetName() const;
    int GetDefense() const;
    int GetAttack() const;
};

const Item NO_ITEM {"no item", 0, 0};

std::ostream& operator<<(std::ostream& os, const Item& item);
std::istream& operator>>(std::istream& is, Item& item);
