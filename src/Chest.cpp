#include "Chest.hpp"

Chest::Chest(const Point& pos, const Item& it): position(pos), isOpen(false), item(it) {}

bool Chest::operator==(const Chest& other) const{
    return this->position == other.position 
        && this->isOpen == other.isOpen 
        && this->item == other.item;
}

Point Chest::GetPosition() const{
    return position;
}

bool Chest::IsOpen() const{
    return this->isOpen;
}

Item Chest::GetItem() const{
    return this->item;
}

Item Chest::Open(){
    Item tmp = this->item;
    item = NO_ITEM;
    this->isOpen = true;
    return tmp;
}

std::ostream& operator<<(std::ostream& os, const Chest& chest) {
    char sep = ':';
    os << chest.GetPosition() << sep << chest.IsOpen() << sep << chest.GetItem();
    return os;
}

std::istream& operator>>(std::istream& is, Chest& chest) {
    char sep;
    Point pos;
    bool isOpen;
    Item item;
    is >> pos >> sep >> isOpen >> sep >> item;
    chest = Chest(pos, item);
    if (isOpen) {
        chest.Open();
    }
    return is;
}