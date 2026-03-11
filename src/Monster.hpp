#pragma once

#include "Character.hpp"

/**
 * @brief Monster/Enemy character class
 * 
 * Represents hostile creatures with AI behavior patterns.
 * Monsters can patrol, chase players, and attack.
 */
class Monster : public Character {
    int experienceReward;
    Item dropItem;
    Point homePosition;
    bool returningHome;

public:
    /** @brief Default constructor */
    Monster()
        : Character(),
          experienceReward(20),
          dropItem(NO_ITEM),
          homePosition(position),
          returningHome(false) {}
    
    /**
     * @brief Constructor with parameters
     * @param pos Starting position
     * @param name Monster's name
     * @param hp Initial health points
     * @param ag Agility stat
     * @param str Strength stat  
     * @param vr View radius
     * @param xp Experience reward when defeated
     * @param drop Item dropped when defeated
     */
    Monster(
        const Point& pos,
        const std::string& name,
        int hp = 50,
        int ag = 8,
        int str = 15,
        int vr = 3,
        int xp = 20,
        const Item& drop = NO_ITEM
    ) : Character(pos, name, hp, ag, str, vr),
        experienceReward(xp),
        dropItem(drop),
        homePosition(pos),
        returningHome(false) {}

    /**
     * @brief Get rendering symbol for monster
     * @return 'M' for Monster
     */
    virtual char GetSymbol() const override { return 'M'; }
    
    /** @brief Patrol behavior (AI) */
    void Patrol();
    
    /**
     * @brief Chase player behavior (AI)
     * @param playerPos Current position of the player
     */
    void ChasePlayer(const Point& playerPos);
    
    /** @brief Attack player behavior (AI) */
    void AttackPlayer();

    /** @brief Experience granted for defeating this monster */
    int GetExperienceReward() const { return experienceReward; }

    /** @brief Loot generated when this monster is defeated */
    Item GetDropItem() const;

    /** @brief Original spawn/home position for return behavior */
    Point GetHomePosition() const { return homePosition; }
    void SetHomePosition(const Point& pos) { homePosition = pos; }

    /** @brief Whether monster is currently returning to home after losing player */
    bool IsReturningHome() const { return returningHome; }
    void SetReturningHome(bool value) { returningHome = value; }
};
