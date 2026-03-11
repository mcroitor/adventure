#include "Player.hpp"

#include <algorithm>
#include <cstdlib>

void Player::Move(Direction direction) {
    Point delta = DirectionToDelta(direction);
    position.x += delta.x;
    position.y += delta.y;

    // Count traveled tiles per successful move (including diagonal movement as 1 tile).
    distanceTravelled += std::max(std::abs(delta.x), std::abs(delta.y));
}

void Player::PickUpItem(const Item& item) {
    if (IsInventoryFull()) {
        return;
    }

    inventory.push_back(item);
    itemsCollected += 1;
}

void Player::UseItem(int index) {
    if (index < 0 || index >= static_cast<int>(inventory.size())) {
        return;
    }

    Item item = inventory[index];
    switch (item.GetType()) {
    case Item::Type::HELM:
        helm = item;
        break;
    case Item::Type::ARMOR:
        armor = item;
        break;
    case Item::Type::WEAPON:
        weapon = item;
        break;
    case Item::Type::BOOTS:
        boots = item;
        break;
    case Item::Type::SHIELD:
        shield = item;
        break;
    case Item::Type::POTION:
        health += 20; // Example: potion heals 20 health
        if (health > maxHealth) health = maxHealth;
        break;
    default:
        break;
    }

    inventory.erase(inventory.begin() + index);
}

void Player::Interact() {}

int Player::Attack(Character& target) {
    if (!target.IsAlive()) {
        return target.GetHealth();
    }

    int healthBefore = target.GetHealth();
    int healthAfter = target.Hit(GetAttack());
    if (healthBefore > 0 && healthAfter == 0) {
        monstersKilled += 1;
    }

    return healthAfter;
}

void Player::GainExperience(int amount) {
    if (amount <= 0) {
        return;
    }

    experience += amount;
    while (experience >= experienceToNextLevel) {
        experience -= experienceToNextLevel;
        level += 1;
        unspentStatPoints += 3;

        experienceToNextLevel += 50;
    }
}

bool Player::AllocateStatPoint(StatType stat) {
    if (unspentStatPoints <= 0) {
        return false;
    }

    switch (stat) {
    case StatType::STRENGTH:
        strength += 1;
        break;
    case StatType::AGILITY:
        agility += 1;
        break;
    case StatType::VITALITY:
        maxHealth += 5;
        health += 5;
        if (health > maxHealth) {
            health = maxHealth;
        }
        break;
    default:
        return false;
    }

    unspentStatPoints -= 1;
    return true;
}

void Player::AddPlayTimeSeconds(int seconds) {
    if (seconds <= 0) {
        return;
    }

    playTimeSeconds += seconds;
}

void Player::AddQuest(const Quest& quest) {
    quests.push_back(quest);
}

Quest* Player::FindQuestByGiver(const std::string& giverName) {
    for (auto& quest : quests) {
        if (quest.GetGiver() == giverName && !quest.IsRewarded()) {
            return &quest;
        }
    }

    return nullptr;
}

const Quest* Player::FindQuestByGiver(const std::string& giverName) const {
    for (const auto& quest : quests) {
        if (quest.GetGiver() == giverName && !quest.IsRewarded()) {
            return &quest;
        }
    }

    return nullptr;
}

int Player::UpdateQuestProgress(Quest::Type type, int amount) {
    if (amount <= 0) {
        return 0;
    }

    int newlyCompleted = 0;
    for (auto& quest : quests) {
        if (quest.IsRewarded() || quest.IsCompleted() || quest.GetType() != type) {
            continue;
        }

        quest.AddProgress(amount);
        if (quest.IsCompleted()) {
            ++newlyCompleted;
        }
    }

    return newlyCompleted;
}

int Player::UpdateReachQuestProgress(const Point& position) {
    int newlyCompleted = 0;
    for (auto& quest : quests) {
        if (quest.IsRewarded() || quest.IsCompleted() || quest.GetType() != Quest::Type::REACH_POSITION) {
            continue;
        }

        quest.UpdatePosition(position);
        if (quest.IsCompleted()) {
            ++newlyCompleted;
        }
    }

    return newlyCompleted;
}