#pragma once

#include <iostream>

#include "MapObject.hpp"
#include "Item.hpp"

/**
 * @brief Treasure chest object
 * 
 * Represents a container that holds an item. Can be opened by the player
 * to retrieve the item inside.
 */
class Chest : public MapObject {
    bool isOpen;  /**< Whether the chest has been opened */
    Item item;    /**< Item contained in the chest */

public:
    /** @brief Default constructor, creates empty closed chest */
    Chest() : MapObject(), isOpen(false), item(NO_ITEM) {}
    
    /**
     * @brief Constructor with position and item
     * @param pos Position on the map
     * @param it Item to place in the chest
     */
    Chest(const Point& pos, const Item& it) : MapObject(pos, 0), isOpen(false), item(it) {}
    Chest(const Chest& other) = default;

    Chest& operator=(const Chest& other) = default;
    
    /**
     * @brief Equality comparison operator
     * @param other Chest to compare with
     * @return true if position, state, and item match
     */
    bool operator==(const Chest& other) const;

    /**
     * @brief Get rendering symbol
     * @return 'C' if closed, 'O' if opened
     */
    virtual char GetSymbol() const override {
        return isOpen ? 'O' : 'C';
    }
    
    /**
     * @brief Chest is passable when opened
     * @return true if open, false if closed
     */
    virtual bool IsPassable() const override { return isOpen; }

    /**
     * @brief Check if chest is opened
     * @return true if opened
     */
    bool IsOpen() const { return isOpen; }
    
    /**
     * @brief Get the item in the chest without opening
     * @return Item contained in chest
     */
    Item GetItem() const { return item; }
    
    /**
     * @brief Open the chest and retrieve its item
     * @return The item that was in the chest
     * 
     * After opening, the chest becomes empty and the isOpen flag is set.
     */
    Item Open();
};

std::ostream& operator<<(std::ostream& os, const Chest& chest);
std::istream& operator>>(std::istream& is, Chest& chest);