#pragma once

#include <iostream>

#include "MapObject.hpp"

/**
 * @brief Obstacle/Barrier on the game map
 * 
 * Represents impassable objects like rocks, trees, water, and walls.
 * Each obstacle has a type that determines its appearance.
 */
class Obstacle : public MapObject {
public:
    /**
     * @brief Types of obstacles
     */
    enum class Type {
        ROCK = 0,  /**< Rock obstacle (R) */
        TREE = 1,  /**< Tree obstacle (T) */
        WATER = 2, /**< Water obstacle (W) */
        WALL = 3   /**< Wall obstacle (#) */
    };

    /** @brief Default constructor */
    Obstacle() : MapObject() {}
    
    /**
     * @brief Constructor with type and position
     * @param t Obstacle type (cast from Type enum)
     * @param pos Position on the map
     */
    Obstacle(int t, const Point& pos) : MapObject(pos, t) {}
    
    /** @brief Copy constructor */
    Obstacle(const Obstacle& other) = default;
    
    Obstacle& operator=(const Obstacle& other) = default;
    
    /**
     * @brief Equality comparison operator
     * @param other Obstacle to compare with
     * @return true if type and position match
     */
    bool operator==(const Obstacle& other) const;

    /**
     * @brief Get rendering symbol based on obstacle type
     * @return 'R' for rock, 'T' for tree, 'W' for water, '#' for wall
     */
    virtual char GetSymbol() const override {
        switch(static_cast<Type>(type)) {
            case Type::ROCK: return 'R';
            case Type::TREE: return 'T';
            case Type::WATER: return 'W';
            case Type::WALL: return '#';
            default: return '?';
        }
    }
    
    /**
     * @brief Obstacles are never passable
     * @return Always false
     */
    virtual bool IsPassable() const override { return false; }
};

std::ostream& operator<<(std::ostream& os, const Obstacle& obstacle);
std::istream& operator>>(std::istream& is, Obstacle& obstacle);