#pragma once

#include "Item.hpp"
#include "Point.hpp"

class Chest {
    Point position;
    bool isOpen;
    Item item;
public:
    Point GetPosition() const;
};