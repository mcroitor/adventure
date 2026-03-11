#pragma once

#include <vector>

#include "Character.hpp"
#include "Direction.hpp"
#include "Quest.hpp"

class Map;

/**
 * @brief Player character class
 * 
 * Represents the player-controlled character with inventory management,
 * movement, item usage, and interaction capabilities.
 */
class Player : public Character {
public:
    enum class StatType {
        STRENGTH,
        AGILITY,
        VITALITY
    };

private:
    std::vector<Item> inventory;  /**< Player's inventory */
    int maxInventorySize;          /**< Maximum number of items in inventory */
    int level;
    int experience;
    int experienceToNextLevel;
    int unspentStatPoints;
    int monstersKilled;
    int distanceTravelled;
    int itemsCollected;
    int playTimeSeconds;
    std::vector<Quest> quests;

    /**
     * @brief Move player in specified direction
     * @param direction Direction to move
     */
    void Move(Direction direction);

    friend class Map;

public:
    /** @brief Default constructor */
    Player()
        : Character(),
          maxInventorySize(20),
          level(1),
          experience(0),
          experienceToNextLevel(100),
          unspentStatPoints(0),
          monstersKilled(0),
          distanceTravelled(0),
          itemsCollected(0),
          playTimeSeconds(0),
          quests() {}
    
    /**
     * @brief Constructor with parameters
     * @param pos Starting position
     * @param name Player's name
     * @param hp Initial health points
     * @param ag Agility stat
     * @param str Strength stat
     * @param vr View radius
     */
    Player(const Point& pos, const std::string& name, int hp = 100, int ag = 10, int str = 10, int vr = 5)
                : Character(pos, name, hp, ag, str, vr),
                    maxInventorySize(20),
                    level(1),
                    experience(0),
                    experienceToNextLevel(100),
                      unspentStatPoints(0),
                    monstersKilled(0),
                    distanceTravelled(0),
                    itemsCollected(0),
                    playTimeSeconds(0),
                    quests() {}

    /**
     * @brief Get rendering symbol for player
     * @return 'H' for Hero
     */
    virtual char GetSymbol() const override { return 'H'; }

    /**
     * @brief Add item to inventory
     * @param item Item to pick up
     */
    void PickUpItem(const Item& item);
    
    /**
     * @brief Use item from inventory
     * @param index Index of item in inventory (0-based)
     * 
     * Equips items based on type or uses consumables (potions).
     * Removes item from inventory after use.
     */
    void UseItem(int index);
    
    /** @brief Interact with nearby objects (chests, NPCs) */
    void Interact();
    
    /**
     * @brief Attack a target character
     * @param target Character to attack
     * @return Target remaining health after the hit
     */
    int Attack(Character& target);

    /**
     * @brief Add experience and perform level-up when threshold is reached
     * @param amount Experience points gained
     */
    void GainExperience(int amount);
    bool AllocateStatPoint(StatType stat);

    void AddPlayTimeSeconds(int seconds);

    int GetLevel() const { return level; }
    int GetExperience() const { return experience; }
    int GetExperienceToNextLevel() const { return experienceToNextLevel; }
    int GetUnspentStatPoints() const { return unspentStatPoints; }
    int GetMonstersKilled() const { return monstersKilled; }
    int GetDistanceTravelled() const { return distanceTravelled; }
    int GetItemsCollected() const { return itemsCollected; }
    int GetPlayTimeSeconds() const { return playTimeSeconds; }

    const std::vector<Quest>& GetQuests() const { return quests; }
    std::vector<Quest>& GetQuests() { return quests; }
    void AddQuest(const Quest& quest);
    Quest* FindQuestByGiver(const std::string& giver);
    const Quest* FindQuestByGiver(const std::string& giver) const;
    int UpdateQuestProgress(Quest::Type type, int amount = 1);
    int UpdateReachQuestProgress(const Point& position);
    
    /**
     * @brief Get reference to inventory
     * @return Const reference to inventory vector
     */
    const std::vector<Item>& GetInventory() const { return inventory; }
    
    /**
     * @brief Get current inventory size
     * @return Number of items in inventory
     */
    int GetInventorySize() const { return inventory.size(); }
    
    /**
     * @brief Get maximum inventory capacity
     * @return Maximum number of items
     */
    int GetMaxInventorySize() const { return maxInventorySize; }
    
    /**
     * @brief Check if inventory is full
     * @return true if inventory is at maximum capacity
     */
    bool IsInventoryFull() const { return inventory.size() >= static_cast<size_t>(maxInventorySize); }
};