#pragma once

#include "Player.hpp"
#include "Monster.hpp"
#include "Chest.hpp"
#include "Obstacle.hpp"

class Map {
    int width;
    int height;
    Player player;

    Monster monsters[10];
    Chest chests[10];
    Obstacle obstacles[10];
public:
    Map() = default;

    int GetWidth() const;
    int GetHeight() const;
};