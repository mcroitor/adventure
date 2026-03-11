/**
 * @file Map.cpp
 * @brief Implementation of Map class
 * Handles map generation, object placement, and serialization.
 * @author Mihail Croitor
 * @date 2025
 * @version 1.0
 */

#include "Map.hpp"

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <deque>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <queue>
#include <sstream>
#include <string>

namespace {

constexpr const char* kSavePath = "map_save.txt";
constexpr const char* kSaveMagic = "ADVENTURE_SAVE_V1";

size_t CellIndex(int width, int x, int y) {
    return static_cast<size_t>(y * width + x);
}

bool IsWithinViewRadius(const Point& center, int radius, int x, int y) {
    return std::abs(center.x - x) <= radius && std::abs(center.y - y) <= radius;
}

bool IsAdjacent(const Point& a, const Point& b) {
    return std::abs(a.x - b.x) <= 1 && std::abs(a.y - b.y) <= 1;
}

int Sign(int value) {
    if (value > 0) {
        return 1;
    }
    if (value < 0) {
        return -1;
    }
    return 0;
}

void PushMessage(std::deque<std::string>& log, const std::string& message) {
    constexpr size_t kMaxMessages = 24;
    log.push_back(message);
    if (log.size() > kMaxMessages) {
        log.pop_front();
    }
}

void WriteItem(std::ostream& os, const Item& item) {
    os << std::quoted(item.GetName()) << ' '
       << item.GetDefense() << ' '
       << item.GetAttack() << ' '
       << static_cast<int>(item.GetType());
}

bool ReadItem(std::istream& is, Item& item) {
    std::string name;
    int defense = 0;
    int attack = 0;
    int typeInt = static_cast<int>(Item::Type::NONE);
    if (!(is >> std::quoted(name) >> defense >> attack >> typeInt)) {
        return false;
    }

    if (typeInt < static_cast<int>(Item::Type::NONE) || typeInt > static_cast<int>(Item::Type::QUEST)) {
        typeInt = static_cast<int>(Item::Type::NONE);
    }

    item = Item(name, defense, attack, static_cast<Item::Type>(typeInt));
    return true;
}

void WriteQuest(std::ostream& os, const Quest& quest) {
    os << "QUEST "
       << quest.GetId() << ' '
       << std::quoted(quest.GetGiver()) << ' '
       << std::quoted(quest.GetTitle()) << ' '
       << std::quoted(quest.GetDescription()) << ' '
       << static_cast<int>(quest.GetType()) << ' '
       << quest.GetTargetAmount() << ' '
       << quest.GetCurrentAmount() << ' '
       << quest.GetRewardExperience() << ' '
       << (quest.IsCompleted() ? 1 : 0) << ' '
       << (quest.IsRewarded() ? 1 : 0) << ' '
       << (quest.HasTargetPoint() ? 1 : 0) << ' '
       << quest.GetTargetPoint().x << ' '
       << quest.GetTargetPoint().y;
}

bool ReadQuest(std::istream& is, Quest& quest) {
    std::string tag;
    int id = 0;
    std::string giver;
    std::string title;
    std::string description;
    int typeInt = 0;
    int targetAmount = 1;
    int currentAmount = 0;
    int rewardExperience = 0;
    int completed = 0;
    int rewarded = 0;
    int hasTargetPoint = 0;
    int targetX = 0;
    int targetY = 0;

    if (!(is >> tag >> id >> std::quoted(giver) >> std::quoted(title) >> std::quoted(description)
          >> typeInt >> targetAmount >> currentAmount >> rewardExperience
          >> completed >> rewarded >> hasTargetPoint >> targetX >> targetY)) {
        return false;
    }

    if (tag != "QUEST") {
        return false;
    }

    if (typeInt < static_cast<int>(Quest::Type::KILL_MONSTERS) ||
        typeInt > static_cast<int>(Quest::Type::REACH_POSITION)) {
        typeInt = static_cast<int>(Quest::Type::KILL_MONSTERS);
    }

    quest = Quest(
        id,
        giver,
        title,
        description,
        static_cast<Quest::Type>(typeInt),
        targetAmount,
        rewardExperience,
        hasTargetPoint != 0,
        Point{targetX, targetY}
    );
    quest.SetProgress(currentAmount, completed != 0);
    quest.SetRewarded(rewarded != 0);
    return true;
}

}

Map::Map(int w, int h) : width(w), height(h) {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

int Map::GetHeight() const {
    return height;
}

int Map::GetWidth() const {
    return width;
}

bool Map::IsInsideMap(int x, int y) const {
    return x >= 0 && y >= 0 && x < width && y < height;
}

bool Map::IsExplored(const std::vector<bool>& explored, int x, int y) const {
    if (!IsInsideMap(x, y)) {
        return false;
    }

    size_t index = CellIndex(width, x, y);
    if (index >= explored.size()) {
        return false;
    }

    return explored[index];
}

void Map::MarkVisibleAsExplored(std::vector<bool>& explored) const {
    Point playerPos = player.GetPosition();
    int viewRadius = player.ViewRadius();

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (IsWithinViewRadius(playerPos, viewRadius, x, y)) {
                size_t index = CellIndex(width, x, y);
                if (index < explored.size()) {
                    explored[index] = true;
                }
            }
        }
    }
}

Player& Map::GetPlayer() {
    return player;
}

const Player& Map::GetPlayer() const {
    return player;
}

std::vector<Monster>& Map::GetMonsters() {
    return monsters;
}

const std::vector<Monster>& Map::GetMonsters() const {
    return monsters;
}

std::vector<NPC>& Map::GetNpcs() {
    return npcs;
}

const std::vector<NPC>& Map::GetNpcs() const {
    return npcs;
}

bool Map::AreAllMonstersDefeated() const {
    for (const auto& monster : monsters) {
        if (monster.IsAlive()) {
            return false;
        }
    }

    return true;
}

Monster* Map::FindAttackTarget(const Point& position) {
    for (auto& monster : monsters) {
        Point monsterPos = monster.GetPosition();
        if (
            monster.IsAlive() &&
            std::abs(position.x - monsterPos.x) <= 1 &&
            std::abs(position.y - monsterPos.y) <= 1
        ) {
            return &monster;
        }
    }

    return nullptr;
}

NPC* Map::FindInteractableNpc(const Point& position) {
    for (auto& npc : npcs) {
        Point npcPos = npc.GetPosition();
        if (
            std::abs(position.x - npcPos.x) <= 1 &&
            std::abs(position.y - npcPos.y) <= 1
        ) {
            return &npc;
        }
    }

    return nullptr;
}

void Map::ProcessMonstersTurn(std::deque<std::string>& messages) {
    auto& currentPlayer = GetPlayer();
    Point playerPos = currentPlayer.GetPosition();

    const Point patrolDeltas[] = {
        {1, 0},
        {-1, 0},
        {0, 1},
        {0, -1},
        {1, 1},
        {1, -1},
        {-1, 1},
        {-1, -1}
    };

    auto tryPatrolStep = [&](Monster& monster, const Point& playerPosition) {
        Point monsterPos = monster.GetPosition();
        int startIndex = rand() % 8;
        for (int i = 0; i < 8; ++i) {
            const Point& delta = patrolDeltas[(startIndex + i) % 8];
            Point target{monsterPos.x + delta.x, monsterPos.y + delta.y};

            if (target == playerPosition) {
                continue;
            }

            if (IsPassable(target)) {
                monster.SetPosition(target);
                return true;
            }
        }

        return false;
    };

    auto findNextStepBfs = [&](const Point& start, const Point& goal, const Point& playerPosition) {
        if (start == goal || !IsInsideMap(goal.x, goal.y)) {
            return start;
        }

        auto toIndex = [&](const Point& p) {
            return p.y * width + p.x;
        };

        auto toPoint = [&](int index) {
            return Point{index % width, index / width};
        };

        std::vector<int> previous(width * height, -1);
        std::vector<bool> visited(width * height, false);
        std::queue<Point> queue;

        const int startIndex = toIndex(start);
        const int goalIndex = toIndex(goal);

        visited[startIndex] = true;
        queue.push(start);

        while (!queue.empty()) {
            Point current = queue.front();
            queue.pop();

            if (current == goal) {
                break;
            }

            for (const auto& delta : patrolDeltas) {
                Point next{current.x + delta.x, current.y + delta.y};
                if (!IsInsideMap(next.x, next.y) || next == playerPosition) {
                    continue;
                }

                const int nextIndex = toIndex(next);
                if (visited[nextIndex]) {
                    continue;
                }

                if (!(next == goal) && !IsPassable(next)) {
                    continue;
                }

                visited[nextIndex] = true;
                previous[nextIndex] = toIndex(current);
                queue.push(next);
            }
        }

        if (!visited[goalIndex]) {
            return start;
        }

        int cursor = goalIndex;
        while (previous[cursor] != -1 && previous[cursor] != startIndex) {
            cursor = previous[cursor];
        }

        if (previous[cursor] == -1) {
            return start;
        }

        Point step = toPoint(cursor);
        if (step == playerPosition || !IsPassable(step)) {
            return start;
        }

        return step;
    };

    auto findNextChaseStepBfs = [&](const Point& start, const Point& playerPosition) {
        auto toIndex = [&](const Point& p) {
            return p.y * width + p.x;
        };

        auto toPoint = [&](int index) {
            return Point{index % width, index / width};
        };

        std::vector<bool> goalMask(width * height, false);
        int goalCount = 0;
        for (const auto& delta : patrolDeltas) {
            Point goal{playerPosition.x + delta.x, playerPosition.y + delta.y};
            if (!IsInsideMap(goal.x, goal.y)) {
                continue;
            }

            if (goal == start || IsPassable(goal)) {
                goalMask[toIndex(goal)] = true;
                ++goalCount;
            }
        }

        if (goalCount == 0) {
            return start;
        }

        std::vector<int> previous(width * height, -1);
        std::vector<bool> visited(width * height, false);
        std::queue<Point> queue;

        const int startIndex = toIndex(start);
        visited[startIndex] = true;
        queue.push(start);

        int reachedGoalIndex = -1;
        while (!queue.empty()) {
            Point current = queue.front();
            queue.pop();

            const int currentIndex = toIndex(current);
            if (goalMask[currentIndex]) {
                reachedGoalIndex = currentIndex;
                break;
            }

            for (const auto& delta : patrolDeltas) {
                Point next{current.x + delta.x, current.y + delta.y};
                if (!IsInsideMap(next.x, next.y) || next == playerPosition) {
                    continue;
                }

                const int nextIndex = toIndex(next);
                if (visited[nextIndex]) {
                    continue;
                }

                if (!(next == start) && !IsPassable(next)) {
                    continue;
                }

                visited[nextIndex] = true;
                previous[nextIndex] = currentIndex;
                queue.push(next);
            }
        }

        if (reachedGoalIndex == -1) {
            return start;
        }

        int cursor = reachedGoalIndex;
        while (previous[cursor] != -1 && previous[cursor] != startIndex) {
            cursor = previous[cursor];
        }

        if (previous[cursor] == -1) {
            return start;
        }

        Point step = toPoint(cursor);
        if (step == playerPosition || !IsPassable(step)) {
            return start;
        }

        return step;
    };

    for (auto& monster : monsters) {
        if (!monster.IsAlive() || !currentPlayer.IsAlive()) {
            continue;
        }

        Point monsterPos = monster.GetPosition();
        bool canSeePlayer = IsWithinViewRadius(
            monsterPos,
            monster.ViewRadius(),
            playerPos.x,
            playerPos.y
        );

        if (!canSeePlayer) {
            if (monster.IsReturningHome()) {
                Point homePos = monster.GetHomePosition();
                if (monsterPos == homePos) {
                    monster.SetReturningHome(false);
                    continue;
                }

                Point nextStep = findNextStepBfs(monsterPos, homePos, playerPos);
                if (nextStep != monsterPos) {
                    monster.SetPosition(nextStep);
                    if (nextStep == homePos) {
                        monster.SetReturningHome(false);
                    }
                } else {
                    // If path is temporarily blocked, keep moving to avoid deadlocks.
                    tryPatrolStep(monster, playerPos);
                }

                continue;
            }

            tryPatrolStep(monster, playerPos);

            continue;
        }

        monster.SetReturningHome(true);

        if (IsAdjacent(monsterPos, playerPos)) {
            int remainingHealth = currentPlayer.Hit(monster.GetAttack());
            std::ostringstream os;
            os << monster.GetName() << " attacks you, HP left: " << remainingHealth;
            PushMessage(messages, os.str());
            continue;
        }

        Point chaseStep = findNextChaseStepBfs(monsterPos, playerPos);
        if (chaseStep != monsterPos) {
            monster.SetPosition(chaseStep);
            continue;
        }

        // Keep old local fallback when no path is found.
        int dx = Sign(playerPos.x - monsterPos.x);
        int dy = Sign(playerPos.y - monsterPos.y);

        Point candidates[] = {
            {monsterPos.x + dx, monsterPos.y},
            {monsterPos.x, monsterPos.y + dy},
            {monsterPos.x + dx, monsterPos.y + dy}
        };

        for (const auto& target : candidates) {
            if (target == monsterPos || target == playerPos) {
                continue;
            }

            if (IsPassable(target)) {
                monster.SetPosition(target);
                break;
            }
        }
    }
}

std::vector<Chest>& Map::GetChests() {
    return chests;
}

const std::vector<Chest>& Map::GetChests() const {
    return chests;
}

Chest* Map::FindInteractableChest(const Point& position) {
    for (auto& chest : chests) {
        Point chestPos = chest.GetPosition();
        if (
            !chest.IsOpen() &&
            std::abs(position.x - chestPos.x) <= 1 &&
            std::abs(position.y - chestPos.y) <= 1
        ) {
            return &chest;
        }
    }

    return nullptr;
}

std::vector<Obstacle>& Map::GetObstacles() {
    return obstacles;
}

const std::vector<Obstacle>& Map::GetObstacles() const {
    return obstacles;
}

bool Map::IsPositionOccupied(const Point& pos) const {
    // Check player position
    Point playerPos = player.GetPosition();
    if(playerPos.x == pos.x && playerPos.y == pos.y)
        return true;
    
    // Check obstacles
    for(const auto& obs : obstacles) {
        Point obsPos = obs.GetPosition();
        if(obsPos.x == pos.x && obsPos.y == pos.y)
            return true;
    }
    
    // Check monsters
    for(const auto& mon : monsters) {
        Point monPos = mon.GetPosition();
        if(monPos.x == pos.x && monPos.y == pos.y)
            return true;
    }

    // Check NPCs
    for (const auto& npc : npcs) {
        Point npcPos = npc.GetPosition();
        if (npcPos.x == pos.x && npcPos.y == pos.y)
            return true;
    }
    
    // Check chests
    for(const auto& chest : chests) {
        Point chestPos = chest.GetPosition();
        if(chestPos.x == pos.x && chestPos.y == pos.y)
            return true;
    }
    
    return false;
}

Point Map::GetRandomEmptyPosition() {
    if (width <= 2 || height <= 2) {
        return player.GetPosition();
    }

    Point pos;
    for (int attempts = 0; attempts < 1000; ++attempts) {
        pos.x = 1 + rand() % (width - 2);
        pos.y = 1 + rand() % (height - 2);
        if (!IsPositionOccupied(pos)) {
            return pos;
        }
    }

    // Fallback to deterministic scan to guarantee a valid free tile if one exists.
    for (int y = 1; y < height - 1; ++y) {
        for (int x = 1; x < width - 1; ++x) {
            Point candidate{x, y};
            if (!IsPositionOccupied(candidate)) {
                return candidate;
            }
        }
    }

    return player.GetPosition();
}

Item Map::GenerateRandomItem() {
    int itemType = rand() % 6;
    int defense = 5 + rand() % 15;
    int attack = 5 + rand() % 15;
    
    switch(itemType) {
        case 0: return Item("Iron Helm", defense, 0, Item::Type::HELM);
        case 1: return Item("Steel Armor", defense, 0, Item::Type::ARMOR);
        case 2: return Item("Sword", 0, attack, Item::Type::WEAPON);
        case 3: return Item("Leather Boots", defense, 0, Item::Type::BOOTS);
        case 4: return Item("Shield", defense, 0, Item::Type::SHIELD);
        case 5: return Item("Health Potion", 0, 0, Item::Type::POTION);
        default: return NO_ITEM;
    }
}

void Map::Generate() {
    // Clear existing objects
    monsters.clear();
    npcs.clear();
    chests.clear();
    obstacles.clear();
    
    // Create border walls
    for(int x = 0; x < width; x++) {
        obstacles.push_back(Obstacle(static_cast<int>(Obstacle::Type::WALL), {x, 0}));
        obstacles.push_back(Obstacle(static_cast<int>(Obstacle::Type::WALL), {x, height-1}));
    }
    for(int y = 1; y < height-1; y++) {
        obstacles.push_back(Obstacle(static_cast<int>(Obstacle::Type::WALL), {0, y}));
        obstacles.push_back(Obstacle(static_cast<int>(Obstacle::Type::WALL), {width-1, y}));
    }
    
    // Place player in center
    player.SetPosition({width/2, height/2});
    
    // Generate random obstacles (10% of map)
    int obstacleCount = (width * height) / 10;
    for(int i = 0; i < obstacleCount; i++) {
        Point pos = GetRandomEmptyPosition();
        int type = rand() % 3; // ROCK, TREE, or WATER
        obstacles.push_back(Obstacle(type, pos));
    }
    
    // Generate monsters (1% of map)
    int monsterCount = (width * height) / 100;
    for(int i = 0; i < monsterCount; i++) {
        Point pos = GetRandomEmptyPosition();
        int monsterType = rand() % 3;
        
        if(monsterType == 0) {
            monsters.push_back(Monster(pos, "Goblin", 50, 8, 12, 3, 25, Item("Goblin Dagger", 0, 6, Item::Type::WEAPON)));
        } else if(monsterType == 1) {
            monsters.push_back(Monster(pos, "Orc", 80, 6, 18, 4, 40, Item("Orc Shield", 8, 0, Item::Type::SHIELD)));
        } else {
            monsters.push_back(Monster(pos, "Skeleton", 40, 10, 10, 5, 30, Item("Bone Helm", 4, 0, Item::Type::HELM)));
        }
    }
    
    // Generate treasure chests (1% of map)
    int chestCount = (width * height) / 100;
    for(int i = 0; i < chestCount; i++) {
        Point pos = GetRandomEmptyPosition();
        Item randomItem = GenerateRandomItem();
        chests.push_back(Chest(pos, randomItem));
    }

    auto findNearestFreeTile = [&](const Point& center, Point& out) {
        if (width <= 2 || height <= 2) {
            return false;
        }

        const int maxRadius = std::max(width, height);
        for (int radius = 1; radius <= maxRadius; ++radius) {
            for (int dy = -radius; dy <= radius; ++dy) {
                for (int dx = -radius; dx <= radius; ++dx) {
                    if (std::abs(dx) != radius && std::abs(dy) != radius) {
                        continue;
                    }

                    Point candidate{center.x + dx, center.y + dy};
                    if (candidate.x <= 0 || candidate.y <= 0 || candidate.x >= width - 1 || candidate.y >= height - 1) {
                        continue;
                    }

                    if (!IsPositionOccupied(candidate)) {
                        out = candidate;
                        return true;
                    }
                }
            }
        }

        return false;
    };

    // Generate friendly NPCs.
    int npcCount = (width * height) / 1600;
    if (npcCount < 1) {
        npcCount = 1;
    }

    // Guarantee at least one NPC appears near the player in every new map.
    Point nearbyNpcPos;
    if (findNearestFreeTile(player.GetPosition(), nearbyNpcPos)) {
        npcs.emplace_back(
            nearbyNpcPos,
            "Hermit",
            std::vector<std::string>{
                "The fog hides danger.",
                "Move carefully and watch your health."
            }
        );
    }

    for (int i = static_cast<int>(npcs.size()); i < npcCount; ++i) {
        Point pos = GetRandomEmptyPosition();

        int npcType = i % 3;
        if (npcType == 0) {
            npcs.emplace_back(
                pos,
                "Hermit",
                std::vector<std::string>{
                    "The fog hides danger.",
                    "Move carefully and watch your health."
                }
            );
        } else if (npcType == 1) {
            npcs.emplace_back(
                pos,
                "Trader",
                std::vector<std::string>{
                    "Keep some potions ready.",
                    "A good weapon saves lives."
                }
            );
        } else {
            npcs.emplace_back(
                pos,
                "Scout",
                std::vector<std::string>{
                    "Monsters can respawn over time.",
                    "Clear the area before celebrating."
                }
            );
        }
    }
}

bool Map::IsPassable(const Point& pos) const {
    if (pos.x < 0 || pos.y < 0 || pos.x >= width || pos.y >= height) {
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

    for (const auto& npc : npcs) {
        if (npc.GetPosition() == pos && !npc.IsPassable()) {
            return false;
        }
    }

    return true;
}

bool Map::TryMovePlayer(Direction direction) {
    Point currentPosition = player.GetPosition();
    Point delta = DirectionToDelta(direction);
    Point target{currentPosition.x + delta.x, currentPosition.y + delta.y};

    MapPart mapPart = GetMapPart();

    if (!mapPart.IsPositionWithin(target)) {
        return false;
    }

    if (!mapPart.IsPassable(target)) {
        return false;
    }

    if (!IsPassable(target)) {
        return false;
    }

    player.Move(direction);
    return true;
}

MapPart Map::GetMapPart() const {
    auto position = player.GetPosition();
    auto radius = player.ViewRadius();
    MapPart mapPart(position, static_cast<size_t>(radius));
    // Add monsters within radius
    for (const auto& monster : monsters) {
        Point monPos = monster.GetPosition();
        if (std::abs(monPos.x - position.x) <= radius && std::abs(monPos.y - position.y) <= radius) {
            mapPart.AddMonster(monster);
        }
    }
    // Add chests within radius
    for (const auto& chest : chests) {
        Point chestPos = chest.GetPosition();
        if (std::abs(chestPos.x - position.x) <= radius && std::abs(chestPos.y - position.y) <= radius) {
            mapPart.AddChest(chest);
        }
    }
    // Add obstacles within radius
    for (const auto& obstacle : obstacles) {
        Point obsPos = obstacle.GetPosition();
        if (std::abs(obsPos.x - position.x) <= radius && std::abs(obsPos.y - position.y) <= radius) {
            mapPart.AddObstacle(obstacle);
        }
    }
    return mapPart;
}

bool Map::Load() {
    std::ifstream file(kSavePath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open save file for reading." << std::endl;
        return false;
    }

    std::string magic;
    std::getline(file, magic);
    if (magic != kSaveMagic) {
        std::cerr << "Error: Invalid save file format." << std::endl;
        return false;
    }

    std::string tag;
    int loadedWidth = 0;
    int loadedHeight = 0;

    if (!(file >> tag >> loadedWidth >> loadedHeight) || tag != "MAP") {
        std::cerr << "Error: MAP section is missing or invalid." << std::endl;
        return false;
    }

    int px = 0;
    int py = 0;
    std::string playerName;
    int playerHealth = 0;
    int playerMaxHealth = 0;
    int playerAgility = 0;
    int playerStrength = 0;
    int playerViewRadius = 0;
    int playerLevel = 1;
    int playerExperience = 0;
    int playerExperienceToNextLevel = 100;
    int playerUnspentStatPoints = 0;
    int playerMonstersKilled = 0;
    int playerDistanceTravelled = 0;
    int playerItemsCollected = 0;
    int playerPlayTimeSeconds = 0;

    if (!(file >> tag >> px >> py >> std::quoted(playerName)
          >> playerHealth >> playerMaxHealth >> playerAgility >> playerStrength >> playerViewRadius
          >> playerLevel >> playerExperience >> playerExperienceToNextLevel >> playerUnspentStatPoints
          >> playerMonstersKilled >> playerDistanceTravelled >> playerItemsCollected >> playerPlayTimeSeconds)
        || tag != "PLAYER") {
        std::cerr << "Error: PLAYER section is missing or invalid." << std::endl;
                return false;
    }

    Item helm;
    Item armor;
    Item weapon;
    Item boots;
    Item shield;
    if (!(file >> tag) || tag != "PLAYER_EQUIPMENT") {
        std::cerr << "Error: PLAYER_EQUIPMENT section is missing." << std::endl;
        return false;
    }
    if (!ReadItem(file, helm) || !ReadItem(file, armor) || !ReadItem(file, weapon)
        || !ReadItem(file, boots) || !ReadItem(file, shield)) {
        std::cerr << "Error: Failed to load player equipment." << std::endl;
        return false;
    }

    size_t inventoryCount = 0;
    if (!(file >> tag >> inventoryCount) || tag != "PLAYER_INVENTORY") {
        std::cerr << "Error: PLAYER_INVENTORY section is missing or invalid." << std::endl;
        return false;
    }
    std::vector<Item> loadedInventory;
    loadedInventory.reserve(inventoryCount);
    for (size_t i = 0; i < inventoryCount; ++i) {
        Item item;
        if (!ReadItem(file, item)) {
            std::cerr << "Error: Failed to load player inventory item." << std::endl;
            return false;
        }
        loadedInventory.push_back(item);
    }

    std::vector<Quest> loadedQuests;
    if (!(file >> tag)) {
        std::cerr << "Error: Unexpected end of save file." << std::endl;
        return false;
    }

    if (tag == "PLAYER_QUESTS") {
        size_t questCount = 0;
        if (!(file >> questCount)) {
            std::cerr << "Error: PLAYER_QUESTS section is invalid." << std::endl;
            return false;
        }

        loadedQuests.reserve(questCount);
        for (size_t i = 0; i < questCount; ++i) {
            Quest quest;
            if (!ReadQuest(file, quest)) {
                std::cerr << "Error: Failed to load player quest." << std::endl;
                return false;
            }
            loadedQuests.push_back(quest);
        }

        if (!(file >> tag)) {
            std::cerr << "Error: MONSTERS section is missing or invalid." << std::endl;
            return false;
        }
    }

    size_t monsterCount = 0;
    if (tag != "MONSTERS" || !(file >> monsterCount)) {
        std::cerr << "Error: MONSTERS section is missing or invalid." << std::endl;
        return false;
    }
    std::vector<Monster> loadedMonsters;
    loadedMonsters.reserve(monsterCount);
    for (size_t i = 0; i < monsterCount; ++i) {
        int mx = 0;
        int my = 0;
        std::string monsterName;
        int monsterHealth = 0;
        int monsterMaxHealth = 0;
        int monsterAgility = 0;
        int monsterStrength = 0;
        int monsterViewRadius = 0;
        int monsterExperienceReward = 0;

        if (!(file >> tag >> mx >> my >> std::quoted(monsterName)
              >> monsterHealth >> monsterMaxHealth >> monsterAgility
              >> monsterStrength >> monsterViewRadius >> monsterExperienceReward)
            || tag != "MONSTER") {
            std::cerr << "Error: MONSTER entry is invalid." << std::endl;
                        return false;
        }

        Item dropItem;
        if (!ReadItem(file, dropItem)) {
            std::cerr << "Error: Failed to load monster drop item." << std::endl;
            return false;
        }

        Monster monster(
            Point{mx, my},
            monsterName,
            (monsterMaxHealth > 0) ? monsterMaxHealth : 1,
            monsterAgility,
            monsterStrength,
            (monsterViewRadius > 0) ? monsterViewRadius : 1,
            monsterExperienceReward,
            dropItem
        );
        monster.SetCoreState(
            monsterName,
            monsterHealth,
            (monsterMaxHealth > 0) ? monsterMaxHealth : 1,
            monsterAgility,
            monsterStrength,
            (monsterViewRadius > 0) ? monsterViewRadius : 1
        );

        loadedMonsters.push_back(monster);
    }

    size_t chestCount = 0;
    if (!(file >> tag >> chestCount) || tag != "CHESTS") {
        std::cerr << "Error: CHESTS section is missing or invalid." << std::endl;
        return false;
    }
    std::vector<Chest> loadedChests;
    loadedChests.reserve(chestCount);
    for (size_t i = 0; i < chestCount; ++i) {
        int cx = 0;
        int cy = 0;
        int openFlag = 0;
        if (!(file >> tag >> cx >> cy >> openFlag) || tag != "CHEST") {
            std::cerr << "Error: CHEST entry is invalid." << std::endl;
            return false;
        }

        Item chestItem;
        if (!ReadItem(file, chestItem)) {
            std::cerr << "Error: Failed to load chest item." << std::endl;
            return false;
        }

        Chest chest(Point{cx, cy}, chestItem);
        if (openFlag != 0) {
            chest.Open();
        }
        loadedChests.push_back(chest);
    }

    size_t obstacleCount = 0;
    if (!(file >> tag >> obstacleCount) || tag != "OBSTACLES") {
        std::cerr << "Error: OBSTACLES section is missing or invalid." << std::endl;
        return false;
    }
    std::vector<Obstacle> loadedObstacles;
    loadedObstacles.reserve(obstacleCount);
    for (size_t i = 0; i < obstacleCount; ++i) {
        int ox = 0;
        int oy = 0;
        int obstacleType = 0;
        if (!(file >> tag >> ox >> oy >> obstacleType) || tag != "OBSTACLE") {
            std::cerr << "Error: OBSTACLE entry is invalid." << std::endl;
            return false;
        }
        loadedObstacles.emplace_back(obstacleType, Point{ox, oy});
    }

    std::vector<NPC> loadedNpcs;

    if (!(file >> tag)) {
        std::cerr << "Error: Unexpected end of save file." << std::endl;
        return false;
    }

    if (tag == "NPCS") {
        size_t npcCount = 0;
        if (!(file >> npcCount)) {
            std::cerr << "Error: NPCS section is missing or invalid." << std::endl;
            return false;
        }

        loadedNpcs.reserve(npcCount);
        for (size_t i = 0; i < npcCount; ++i) {
            int nx = 0;
            int ny = 0;
            std::string npcName;
            size_t dialogueCursor = 0;
            size_t dialogueCount = 0;

            if (!(file >> tag >> nx >> ny >> std::quoted(npcName) >> dialogueCursor >> dialogueCount)
                || tag != "NPC") {
                std::cerr << "Error: NPC entry is invalid." << std::endl;
                return false;
            }

            std::vector<std::string> npcDialogues;
            npcDialogues.reserve(dialogueCount);
            for (size_t j = 0; j < dialogueCount; ++j) {
                std::string line;
                if (!(file >> std::quoted(line))) {
                    std::cerr << "Error: Failed to load NPC dialogue line." << std::endl;
                    return false;
                }
                npcDialogues.push_back(line);
            }

            NPC npc(Point{nx, ny}, npcName, npcDialogues);
            npc.SetDialogueCursor(dialogueCursor);
            loadedNpcs.push_back(npc);
        }

        if (!(file >> tag)) {
            std::cerr << "Error: END marker is missing." << std::endl;
            return false;
        }
    }

    if (tag != "END") {
        std::cerr << "Error: END marker is missing." << std::endl;
        return false;
    }

    width = loadedWidth;
    height = loadedHeight;

    Player loadedPlayer(
        Point{px, py},
        playerName,
        (playerMaxHealth > 0) ? playerMaxHealth : 1,
        playerAgility,
        playerStrength,
        (playerViewRadius > 0) ? playerViewRadius : 1
    );
    loadedPlayer.SetCoreState(
        playerName,
        playerHealth,
        (playerMaxHealth > 0) ? playerMaxHealth : 1,
        playerAgility,
        playerStrength,
        (playerViewRadius > 0) ? playerViewRadius : 1
    );
    loadedPlayer.SetEquipment(helm, armor, weapon, boots, shield);

    loadedPlayer.level = playerLevel;
    loadedPlayer.experience = playerExperience;
    loadedPlayer.experienceToNextLevel = playerExperienceToNextLevel;
    loadedPlayer.unspentStatPoints = playerUnspentStatPoints;
    loadedPlayer.monstersKilled = playerMonstersKilled;
    loadedPlayer.distanceTravelled = playerDistanceTravelled;
    loadedPlayer.itemsCollected = playerItemsCollected;
    loadedPlayer.playTimeSeconds = playerPlayTimeSeconds;
    loadedPlayer.inventory = loadedInventory;
    loadedPlayer.quests = loadedQuests;

    player = loadedPlayer;
    monsters = loadedMonsters;
    npcs = loadedNpcs;
    chests = loadedChests;
    obstacles = loadedObstacles;

    return true;
}

bool Map::Save() const {
    std::ofstream file(kSavePath);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for writing." << std::endl;
        return false;
    }

    file << kSaveMagic << '\n';
    file << "MAP " << width << ' ' << height << '\n';

    Point playerPos = player.GetPosition();
    file << "PLAYER "
         << playerPos.x << ' '
         << playerPos.y << ' '
         << std::quoted(player.GetName()) << ' '
         << player.GetHealth() << ' '
         << player.GetMaxHealth() << ' '
         << player.GetAgility() << ' '
         << player.GetStrength() << ' '
         << player.ViewRadius() << ' '
         << player.GetLevel() << ' '
         << player.GetExperience() << ' '
         << player.GetExperienceToNextLevel() << ' '
         << player.GetUnspentStatPoints() << ' '
         << player.GetMonstersKilled() << ' '
         << player.GetDistanceTravelled() << ' '
         << player.GetItemsCollected() << ' '
         << player.GetPlayTimeSeconds()
         << '\n';

    file << "PLAYER_EQUIPMENT\n";
    WriteItem(file, player.GetHelm());
    file << '\n';
    WriteItem(file, player.GetArmor());
    file << '\n';
    WriteItem(file, player.GetWeapon());
    file << '\n';
    WriteItem(file, player.GetBoots());
    file << '\n';
    WriteItem(file, player.GetShield());
    file << '\n';

    file << "PLAYER_INVENTORY " << player.GetInventory().size() << '\n';
    for (const auto& item : player.GetInventory()) {
        WriteItem(file, item);
        file << '\n';
    }

    file << "PLAYER_QUESTS " << player.GetQuests().size() << '\n';
    for (const auto& quest : player.GetQuests()) {
        WriteQuest(file, quest);
        file << '\n';
    }

    file << "MONSTERS " << monsters.size() << '\n';
    for (const auto& monster : monsters) {
        Point pos = monster.GetPosition();
        file << "MONSTER "
             << pos.x << ' '
             << pos.y << ' '
             << std::quoted(monster.GetName()) << ' '
             << monster.GetHealth() << ' '
             << monster.GetMaxHealth() << ' '
             << monster.GetAgility() << ' '
             << monster.GetStrength() << ' '
             << monster.ViewRadius() << ' '
             << monster.GetExperienceReward()
             << '\n';
        WriteItem(file, monster.GetDropItem());
        file << '\n';
    }

    file << "CHESTS " << chests.size() << '\n';
    for (const auto& chest : chests) {
        Point pos = chest.GetPosition();
        file << "CHEST "
             << pos.x << ' '
             << pos.y << ' '
             << (chest.IsOpen() ? 1 : 0)
             << '\n';
        WriteItem(file, chest.GetItem());
        file << '\n';
    }

    file << "OBSTACLES " << obstacles.size() << '\n';
    for (const auto& obstacle : obstacles) {
        Point pos = obstacle.GetPosition();
        file << "OBSTACLE "
             << pos.x << ' '
             << pos.y << ' '
             << obstacle.GetType()
             << '\n';
    }

    file << "NPCS " << npcs.size() << '\n';
    for (const auto& npc : npcs) {
        Point pos = npc.GetPosition();
        const auto& dialogues = npc.GetDialogues();

        file << "NPC "
             << pos.x << ' '
             << pos.y << ' '
             << std::quoted(npc.GetName()) << ' '
             << npc.GetDialogueCursor() << ' '
             << dialogues.size()
             << '\n';

        for (const auto& line : dialogues) {
            file << std::quoted(line) << '\n';
        }
    }

    file << "END\n";

    return static_cast<bool>(file);
}
