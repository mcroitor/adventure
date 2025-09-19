#include "Chest.hpp"

Point Chest::GetPosition() const{
    return position;
}

bool Chest::IsOpen() const{
    return this->isOpen;
}

Item Chest::Open(){
    Item tmp = this->item;
    item = NO_ITEM;
    return tmp;
}