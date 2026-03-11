#pragma once

#include "Point.hpp"
#include "Monster.hpp"
#include "Chest.hpp"
#include "Obstacle.hpp"

#include <cstddef>
#include <vector>

/**
 * @brief A segment of the game map containing entities
 * 
 * Represents a portion of the overall game map, holding monsters,
 * chests, and obstacles within a defined radius from a central position.
 */
class MapPart {
    Point m_position;
    size_t radius;

    std::vector<Monster> monsters;
    std::vector<Chest> chests;
    std::vector<Obstacle> obstacles;
public:

    /**
     * @brief Constructor
     * @param pos Center position of the map part
     * @param r Radius defining the size of the map part
     */
    MapPart(const Point& pos, size_t r);

    /** @brief Get center position of the map part */
    Point GetPosition() const;

    /** @brief Get radius of the map part */
    size_t GetRadius() const;

    /** @brief Get monsters in the map part */
    const std::vector<Monster>& GetMonsters();

    /** @brief Get chests in the map part */
    const std::vector<Chest>& GetChests() const;

    /** @brief Get obstacles in the map part */
    const std::vector<Obstacle>& GetObstacles() const;

    /**
     * @brief Add a monster to the map part
     * @param monster Monster to add
     */
    void AddMonster(const Monster& monster);

    /**
     * @brief Add a chest to the map part
     * @param chest Chest to add
     */
    void AddChest(const Chest& chest);

    /**
     * @brief Add an obstacle to the map part
     * @param obstacle Obstacle to add
     */
    void AddObstacle(const Obstacle& obstacle);

    /**
     * @brief Check if a position is within the map part
     * @param pos Position to check
     * @return true if position is within radius of center
     */
    bool IsPositionWithin(const Point& pos) const;

    /**
     * @brief Check if a position in this map part is passable
     * @param pos Position to check
     * @return true if no blocking object occupies the position
     */
    bool IsPassable(const Point& pos) const;
};