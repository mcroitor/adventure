#include "Item.hpp"

Item::Item(const std::string& name, int def, int att)
{
    name_ = name;
    defense_ = def;
    attack_ = att;
}

int Item::GetAttack() const {
    return attack_;
}

int Item::GetDefense() const {
    return defense_;
}