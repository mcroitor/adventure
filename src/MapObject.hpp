#pragma once

#include "GameObject.hpp"

/**
 * @brief Base class for static objects on the map (Obstacle, Chest)
 * 
 * This class represents non-moving objects that can be placed on the game map.
 * Each object has a type identifier for distinguishing variants.
 */
class MapObject : public GameObject {
protected:
    int type;  /**< Object type identifier (for different obstacle types, etc.) */

public:
    /** @brief Default constructor */
    MapObject() : GameObject(), type(0) {}
    
    /**
     * @brief Constructor with position and type
     * @param pos Position on the map
     * @param t Type identifier
     */
    MapObject(const Point& pos, int t) : GameObject(pos), type(t) {}
    
    virtual ~MapObject() = default;

    /**
     * @brief Get the type identifier of this object
     * @return Type value
     */
    virtual int GetType() const { return type; }
    
    /**
     * @brief Static objects are not passable by default
     * @return false (can be overridden in derived classes)
     */
    virtual bool IsPassable() const override { return false; }
};
