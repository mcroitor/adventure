#pragma once

#include "Point.hpp"

/**
 * @brief Abstract base class for all objects on the game map
 * 
 * This class provides common interface for all game entities including
 * position management, rendering symbol, and passability.
 */
class GameObject {
protected:
    Point position;

public:
    /** @brief Default constructor, initializes position to (0, 0) */
    GameObject() : position{0, 0} {}
    
    /**
     * @brief Constructor with position
     * @param pos Initial position of the object
     */
    GameObject(const Point& pos) : position(pos) {}
    
    virtual ~GameObject() = default;

    /**
     * @brief Get the position of the object
     * @return Current position on the map
     */
    virtual Point GetPosition() const {
        return position;
    }

    /**
     * @brief Set the position of the object
     * @param pos New position on the map
     */
    virtual void SetPosition(const Point& pos) {
        position = pos;
    }

    /**
     * @brief Get the symbol for rendering this object on the map
     * @return Character symbol (must be overridden in derived classes)
     */
    virtual char GetSymbol() const = 0;

    /**
     * @brief Check if the object can be passed through
     * @return true if passable, false otherwise
     */
    virtual bool IsPassable() const = 0;
};
