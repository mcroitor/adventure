#pragma once

#include "GameObject.hpp"
#include "Item.hpp"
#include <string>

/**
 * @brief Base class for all characters in the game (Player, Monster, NPC)
 * 
 * This class extends GameObject with character-specific attributes like health,
 * stats (agility, strength), equipment, and combat mechanics.
 */
class Character : public GameObject {
protected:
    std::string name;      /**< Character name */
    int health;            /**< Current health points */
    int maxHealth;         /**< Maximum health points */
    int agility;           /**< Agility stat (affects speed, dodge) */
    int strength;          /**< Strength stat (affects damage) */
    int viewRadius;        /**< How far the character can see */
    
    // Equipment slots
    Item helm;             /**< Head armor slot */
    Item armor;            /**< Body armor slot */
    Item weapon;           /**< Weapon slot */
    Item boots;            /**< Footwear slot */
    Item shield;           /**< Shield slot */

public:
    Character() 
        : GameObject(), name("Unknown"), health(100), maxHealth(100),
          agility(10), strength(10), viewRadius(5),
          helm(NO_ITEM), armor(NO_ITEM), weapon(NO_ITEM),
          boots(NO_ITEM), shield(NO_ITEM) {}

    Character(const Point& pos, const std::string& n, int hp, int ag, int str, int vr)
        : GameObject(pos), name(n), health(hp), maxHealth(hp),
          agility(ag), strength(str), viewRadius(vr),
          helm(NO_ITEM), armor(NO_ITEM), weapon(NO_ITEM),
          boots(NO_ITEM), shield(NO_ITEM) {}

    virtual ~Character() = default;

    /**
     * @brief Apply damage to the character
     * @param damage Amount of raw damage to apply
     * @return Remaining health after damage
     * 
     * Damage is reduced by total defense from equipment.
     * Health cannot drop below 0.
     */
    virtual int Hit(int damage) {
        int defense = GetDefense();
        if (defense < damage) {
            health -= (damage - defense);
        }
        if (health < 0) {
            health = 0;
        }
        return health;
    }

    /**
     * @brief Restore health up to the maximum value
     * @param amount Amount of HP to restore
     * @return Current health after healing
     */
    virtual int Heal(int amount) {
        if (amount <= 0 || !IsAlive()) {
            return health;
        }

        health += amount;
        if (health > maxHealth) {
            health = maxHealth;
        }

        return health;
    }

    /**
     * @brief Calculate total defense from all equipment
     * @return Sum of defense values from all equipped items
     */
    virtual int GetDefense() const {
        return helm.GetDefense() + 
               armor.GetDefense() + 
               weapon.GetDefense() + 
               boots.GetDefense() + 
               shield.GetDefense();
    }

    /**
     * @brief Calculate total attack power
     * @return Sum of strength stat and attack values from all equipped items
     */
    virtual int GetAttack() const {
        return strength + 
               helm.GetAttack() + 
               armor.GetAttack() + 
               weapon.GetAttack() + 
               boots.GetAttack() + 
               shield.GetAttack();
    }

    /**
     * @brief Get the view radius of the character
     * @return How many tiles the character can see
     */
    virtual int ViewRadius() const { return viewRadius; }
    
    /**
     * @brief Get current health
     * @return Current health points
     */
    virtual int GetHealth() const { return health; }
    
    /**
     * @brief Get maximum health
     * @return Maximum health points
     */
    virtual int GetMaxHealth() const { return maxHealth; }
    
    /**
     * @brief Get agility stat
     * @return Agility value
     */
    virtual int GetAgility() const { return agility; }
    
    /**
     * @brief Get strength stat
     * @return Strength value
     */
    virtual int GetStrength() const { return strength; }

    /** @brief Get base character equipment by slot */
    const Item& GetHelm() const { return helm; }
    const Item& GetArmor() const { return armor; }
    const Item& GetWeapon() const { return weapon; }
    const Item& GetBoots() const { return boots; }
    const Item& GetShield() const { return shield; }

    /**
     * @brief Overwrite core combat/identity state
     * @param newName Display name
     * @param currentHealth Current HP value
     * @param maxHealthValue Maximum HP value
     * @param agilityValue Agility stat
     * @param strengthValue Strength stat
     * @param viewRadiusValue Visibility radius
     */
    void SetCoreState(
        const std::string& newName,
        int currentHealth,
        int maxHealthValue,
        int agilityValue,
        int strengthValue,
        int viewRadiusValue
    ) {
        name = newName;
        maxHealth = (maxHealthValue > 0) ? maxHealthValue : 1;
        health = currentHealth;
        if (health < 0) {
            health = 0;
        }
        if (health > maxHealth) {
            health = maxHealth;
        }
        agility = agilityValue;
        strength = strengthValue;
        viewRadius = (viewRadiusValue > 0) ? viewRadiusValue : 1;
    }

    /**
     * @brief Replace currently equipped items
     */
    void SetEquipment(
        const Item& newHelm,
        const Item& newArmor,
        const Item& newWeapon,
        const Item& newBoots,
        const Item& newShield
    ) {
        helm = newHelm;
        armor = newArmor;
        weapon = newWeapon;
        boots = newBoots;
        shield = newShield;
    }
    
    /**
     * @brief Get character name
     * @return Character's name
     */
    virtual std::string GetName() const { return name; }
    
    /**
     * @brief Check if the character is alive
     * @return true if health > 0, false otherwise
     */
    virtual bool IsAlive() const { return health > 0; }
    
    /**
     * @brief Characters are passable only if dead
     * @return true if dead, false if alive
     */
    virtual bool IsPassable() const override { return !IsAlive(); }
};
