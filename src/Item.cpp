#include "Item.hpp"

Item::Item(const std::string& name, int def, int att)
{
    name_ = name;
    defense_ = def;
    attack_ = att;
}

bool Item::operator==(const Item& other) const {
    return name_ == other.name_
        && defense_ == other.defense_
        && attack_ == other.attack_;
}

std::string Item::GetName() const {
    return name_;
}

int Item::GetAttack() const {
    return attack_;
}

int Item::GetDefense() const {
    return defense_;
}

std::ostream& operator<<(std::ostream& os, const Item& item) {
    os << item.GetName() << ' ' << item.GetDefense() << ' ' << item.GetAttack();
    return os;
}

std::istream& operator>>(std::istream& is, Item& item) {
    std::string name;
    int defense, attack;
    if (is >> name >> defense >> attack) {
        item = Item(name, defense, attack);
    }
    return is;
}