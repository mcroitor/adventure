#include "Map.hpp"

#include <cstdlib>
#include <ctime>

#include <fstream>

int Map::GetHeight() const {
    return height;
}

int Map::GetWidth() const {
    return width;
}

Player& Map::GetPlayer() {
    return player;
}

Monster* Map::GetMonsters() {
    return monsters;
}

Chest* Map::GetChests() {
    return chests;
}

Obstacle* Map::GetObstacles() {
    return obstacles;
}

void Map::Generate() {
    width = 100;
    height = 100;

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    player = Player();

    for (auto& monster : monsters) {
        monster = Monster();
    }

    for (auto& chest : chests) {
        chest = Chest();
    }

    for (auto& obstacle : obstacles) {
        obstacle = Obstacle();
    }
}

void Map::Load() {}

void Map::Save() const {
    // save to file current map state
    std::ofstream file("map_save.txt");

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for writing." << std::endl;
        return;
    }

    // Save map dimensions
    // Save player
    // Save monsters
    // Save chests
    // Save obstacles
}