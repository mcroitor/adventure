#include "Monster.hpp"

Item Monster::GetDropItem() const {
    if (!(dropItem == NO_ITEM)) {
        return dropItem;
    }

    if (name == "Goblin") {
        return Item("Goblin Dagger", 0, 6, Item::Type::WEAPON);
    }
    if (name == "Orc") {
        return Item("Orc Shield", 8, 0, Item::Type::SHIELD);
    }
    if (name == "Skeleton") {
        return Item("Bone Helm", 4, 0, Item::Type::HELM);
    }

    return NO_ITEM;
}

void Monster::Patrol() {
    // TODO: Implement patrol logic
}

void Monster::ChasePlayer(const Point& playerPos) {
    // TODO: Implement chase logic
}

void Monster::AttackPlayer() {
    // TODO: Implement attack logic
}