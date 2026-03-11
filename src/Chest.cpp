#include "Chest.hpp"

bool Chest::operator==(const Chest& other) const{
    return this->position.x == other.position.x 
        && this->position.y == other.position.y
        && this->isOpen == other.isOpen 
        && this->item == other.item;
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