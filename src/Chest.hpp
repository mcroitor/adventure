#pragma once

#include <iostream>

#include "Item.hpp"
#include "Point.hpp"

class Chest {
    Point position;
    bool isOpen;
    Item item;
public:
    Chest() = default;
    Chest(const Point& pos, const Item& it);
    Chest(const Chest& other) = default;

    Chest& operator=(const Chest& other) = default;
    bool operator==(const Chest& other) const;

    Point GetPosition() const;
    bool IsOpen() const;
    Item GetItem() const;
    Item Open();
};

std::ostream& operator<<(std::ostream& os, const Chest& chest);
std::istream& operator>>(std::istream& is, Chest& chest);