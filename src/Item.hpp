#pragma once

#include <string>
#include <iostream>

class Item {
public:
    enum class Type {
        NONE,
        HELM,
        ARMOR,
        WEAPON,
        BOOTS,
        SHIELD,
        POTION,
        QUEST
    };

private:
    std::string name_;
    int defense_;
    int attack_;
    Type type_;

public:
    Item() : name_(""), defense_(0), attack_(0), type_(Type::NONE) {}
    Item(const std::string& name, int def, int att, Type type = Type::NONE);
    Item(const Item& other) = default;

    Item& operator=(const Item& other) = default;
    bool operator==(const Item& other) const;

    std::string GetName() const;
    int GetDefense() const;
    int GetAttack() const;
    Type GetType() const;
};

const Item NO_ITEM {"no item", 0, 0, Item::Type::NONE};

std::ostream& operator<<(std::ostream& os, const Item& item);
std::istream& operator>>(std::istream& is, Item& item);
