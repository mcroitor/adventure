#pragma once

#include "Player.hpp"
#include "Monster.hpp"
#include "NPC.hpp"
#include "Chest.hpp"
#include "Obstacle.hpp"
#include "MapPart.hpp"

#include <deque>
#include <string>
#include <vector>

/**
 * @brief Game world map containing all entities
 * 
 * Manages the game world, including player, monsters, chests, and obstacles.
 * Handles map generation, collision detection, and persistence.
 */
class Map {
    int width;   /**< Map width in tiles */
    int height;  /**< Map height in tiles */
    Player player;

    std::vector<Monster> monsters;
    std::vector<NPC> npcs;
    std::vector<Chest> chests;
    std::vector<Obstacle> obstacles;

private:
    /**
     * @brief Check if a position is occupied by any object
     * @param pos Position to check
     * @return true if position is occupied
     */
    bool IsPositionOccupied(const Point& pos) const;
    
    /**
     * @brief Find a random empty position on the map
     * @return Empty position coordinates
     */
    Point GetRandomEmptyPosition();
    
    /**
     * @brief Generate a random item with random stats
     * @return Generated item
     */
    Item GenerateRandomItem();

public:
    /**
     * @brief Constructor
     * @param w Map width (default 80)
     * @param h Map height (default 40)
     */
    Map(int w = 80, int h = 40);

    /**
     * @brief Get map width
     * @return Map width
     */
    int GetWidth() const;
    
    /**
     * @brief Get map height
     * @return Map height
     */
    int GetHeight() const;
    
    /**
     * @brief Generate map with obstacles, monsters, and chests
     * 
     * Creates border walls, randomly places obstacles, monsters, and treasure chests.
     * Player is placed in the center of the map.
     */
    void Generate();

    /**
     * @brief Check whether a map cell can be traversed
     * @param pos Position to check
     * @return true if the position is in bounds and not blocked
     */
    bool IsPassable(const Point& pos) const;

    /**
     * @brief Check whether coordinates are inside the map bounds
     * @param x X coordinate
     * @param y Y coordinate
     * @return true if coordinates are valid map cell
     */
    bool IsInsideMap(int x, int y) const;

    /**
     * @brief Check whether a cell is already explored in fog-of-war buffer
     * @param explored Explored cells buffer
     * @param x X coordinate
     * @param y Y coordinate
     * @return true if cell is inside map and marked explored
     */
    bool IsExplored(const std::vector<bool>& explored, int x, int y) const;

    /**
     * @brief Mark all currently visible cells as explored
     * @param explored Explored cells buffer to update
     */
    void MarkVisibleAsExplored(std::vector<bool>& explored) const;

    /**
     * @brief Attempt to move player by one tile in a direction
     * @param direction Direction to move
     * @return true if movement succeeded, false if blocked
     */
    bool TryMovePlayer(Direction direction);
    
    /**
     * @brief Get reference to player
     * @return Reference to player object
     */
    const Player& GetPlayer() const;
    Player& GetPlayer();
    
    /**
     * @brief Get reference to monsters vector
     * @return Reference to monsters vector
     */
    const std::vector<Monster>& GetMonsters() const;
    std::vector<Monster>& GetMonsters();

    /**
     * @brief Check whether all monsters on the map are defeated
     * @return true if no alive monsters remain
     */
    bool AreAllMonstersDefeated() const;

    /**
     * @brief Find first alive monster adjacent to a point
     * @param position Center point for adjacency check
     * @return Pointer to target monster or nullptr
     */
    Monster* FindAttackTarget(const Point& position);

    /**
     * @brief Get reference to NPC vector
     * @return Reference to NPC vector
     */
    const std::vector<NPC>& GetNpcs() const;
    std::vector<NPC>& GetNpcs();

    /**
     * @brief Find first NPC adjacent to a point
     * @param position Center point for adjacency check
     * @return Pointer to NPC or nullptr
     */
    NPC* FindInteractableNpc(const Point& position);

    /**
     * @brief Process one monsters turn and append combat log messages
     * @param messages Message log buffer
     */
    void ProcessMonstersTurn(std::deque<std::string>& messages);
    
    /**
     * @brief Get reference to chests vector
     * @return Reference to chests vector
     */
    const std::vector<Chest>& GetChests() const;
    std::vector<Chest>& GetChests();

    /**
     * @brief Find first closed chest adjacent to a point
     * @param position Center point for adjacency check
     * @return Pointer to chest or nullptr
     */
    Chest* FindInteractableChest(const Point& position);
    
    /**
     * @brief Get reference to obstacles vector
     * @return Reference to obstacles vector
     */
    const std::vector<Obstacle>& GetObstacles() const;
    std::vector<Obstacle>& GetObstacles();

    /**
     * @brief Get the MapPart containing the player
     * @return MapPart around the player
     */
    MapPart GetMapPart() const;

    /**
     * @brief Load map from file
     */
    bool Load();
    
    /**
     * @brief Save map to file
     */
    bool Save() const;
};