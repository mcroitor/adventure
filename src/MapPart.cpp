#include "MapPart.hpp"

#include <cmath>

MapPart::MapPart(const Point& pos, size_t r) : m_position(pos), radius(r) {}

Point MapPart::GetPosition() const {
    return m_position;
}

size_t MapPart::GetRadius() const {
    return radius;
}

const std::vector<Monster>& MapPart::GetMonsters() {
    return monsters;
}

const std::vector<Chest>& MapPart::GetChests() const {
    return chests;
}

const std::vector<Obstacle>& MapPart::GetObstacles() const {
    return obstacles;
}

void MapPart::AddMonster(const Monster& monster) {
    monsters.push_back(monster);
}

void MapPart::AddChest(const Chest& chest) {
    chests.push_back(chest);
}

void MapPart::AddObstacle(const Obstacle& obstacle) {
    obstacles.push_back(obstacle);
}

bool MapPart::IsPositionWithin(const Point& pos) const {
    return (std::abs(pos.x - m_position.x) <= static_cast<int>(radius)) &&
           (std::abs(pos.y - m_position.y) <= static_cast<int>(radius));
}

bool MapPart::IsPassable(const Point& pos) const {
    if (!IsPositionWithin(pos)) {
        return false;
    }

    for (const auto& obstacle : obstacles) {
        if (obstacle.GetPosition() == pos && !obstacle.IsPassable()) {
            return false;
        }
    }

    for (const auto& chest : chests) {
        if (chest.GetPosition() == pos && !chest.IsPassable()) {
            return false;
        }
    }

    for (const auto& monster : monsters) {
        if (monster.GetPosition() == pos && !monster.IsPassable()) {
            return false;
        }
    }

    return true;
}