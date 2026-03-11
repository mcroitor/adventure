/**
 * @file test_map.cpp
 * @brief Unit tests for map passability and player movement
 */

#include <catch2/catch_test_macros.hpp>

#include <cmath>
#include <cstdio>
#include <vector>

#include "EventBus.hpp"
#include "Map.hpp"
#include "Direction.hpp"
#include "Item.hpp"

TEST_CASE("Obstacle symbols use terrain glyphs", "[Map][Obstacle]") {
    REQUIRE(Obstacle(static_cast<int>(Obstacle::Type::ROCK), {1, 1}).GetSymbol() == '^');
    REQUIRE(Obstacle(static_cast<int>(Obstacle::Type::TREE), {1, 1}).GetSymbol() == 'T');
    REQUIRE(Obstacle(static_cast<int>(Obstacle::Type::WATER), {1, 1}).GetSymbol() == '~');
    REQUIRE(Obstacle(static_cast<int>(Obstacle::Type::WALL), {1, 1}).GetSymbol() == '#');
}

TEST_CASE("Map::IsPassable validates bounds and blocking objects", "[Map]") {
    Map map(20, 20);

    SECTION("Out of bounds is not passable") {
        REQUIRE_FALSE(map.IsPassable({-1, 0}));
        REQUIRE_FALSE(map.IsPassable({0, -1}));
        REQUIRE_FALSE(map.IsPassable({20, 0}));
        REQUIRE_FALSE(map.IsPassable({0, 20}));
    }

    SECTION("Empty in-bounds tile is passable") {
        REQUIRE(map.IsPassable({5, 5}));
    }

    SECTION("Obstacle blocks movement") {
        map.GetObstacles().push_back(Obstacle(static_cast<int>(Obstacle::Type::ROCK), {6, 5}));
        REQUIRE_FALSE(map.IsPassable({6, 5}));
    }

    SECTION("Closed chest blocks movement") {
        map.GetChests().push_back(Chest({6, 5}, Item("Loot", 0, 0, Item::Type::QUEST)));
        REQUIRE_FALSE(map.IsPassable({6, 5}));
    }

    SECTION("Opened chest is passable") {
        Chest chest({6, 5}, Item("Loot", 0, 0, Item::Type::QUEST));
        chest.Open();
        map.GetChests().push_back(chest);
        REQUIRE(map.IsPassable({6, 5}));
    }

    SECTION("Alive monster blocks movement") {
        map.GetMonsters().push_back(Monster({6, 5}, "Goblin", 30, 5, 5, 3));
        REQUIRE_FALSE(map.IsPassable({6, 5}));
    }

    SECTION("NPC blocks movement") {
        map.GetNpcs().push_back(NPC({6, 5}, "Hermit", {"Hello"}));
        REQUIRE_FALSE(map.IsPassable({6, 5}));
    }
}

TEST_CASE("Map::TryMovePlayer uses collisions for movement", "[Map]") {
    Map map(20, 20);
    map.GetPlayer().SetPosition({5, 5});

    SECTION("Moves to free tile") {
        REQUIRE(map.TryMovePlayer(Direction::EAST));
        REQUIRE(map.GetPlayer().GetPosition() == Point{6, 5});
    }

    SECTION("Blocked by obstacle") {
        map.GetObstacles().push_back(Obstacle(static_cast<int>(Obstacle::Type::ROCK), {6, 5}));

        REQUIRE_FALSE(map.TryMovePlayer(Direction::EAST));
        REQUIRE(map.GetPlayer().GetPosition() == Point{5, 5});
    }

    SECTION("Blocked by closed chest") {
        map.GetChests().push_back(Chest({6, 5}, Item("Loot", 0, 0, Item::Type::QUEST)));

        REQUIRE_FALSE(map.TryMovePlayer(Direction::EAST));
        REQUIRE(map.GetPlayer().GetPosition() == Point{5, 5});
    }

    SECTION("Can step through opened chest") {
        Chest chest({6, 5}, Item("Loot", 0, 0, Item::Type::QUEST));
        chest.Open();
        map.GetChests().push_back(chest);

        REQUIRE(map.TryMovePlayer(Direction::EAST));
        REQUIRE(map.GetPlayer().GetPosition() == Point{6, 5});
    }

    SECTION("Blocked by alive monster") {
        map.GetMonsters().push_back(Monster({6, 5}, "Goblin", 30, 5, 5, 3));

        REQUIRE_FALSE(map.TryMovePlayer(Direction::EAST));
        REQUIRE(map.GetPlayer().GetPosition() == Point{5, 5});
    }

    SECTION("Blocked by NPC") {
        map.GetNpcs().push_back(NPC({6, 5}, "Hermit", {"Hello"}));

        REQUIRE_FALSE(map.TryMovePlayer(Direction::EAST));
        REQUIRE(map.GetPlayer().GetPosition() == Point{5, 5});
    }

    SECTION("Boundary blocks movement") {
        map.GetPlayer().SetPosition({0, 0});

        REQUIRE_FALSE(map.TryMovePlayer(Direction::WEST));
        REQUIRE_FALSE(map.TryMovePlayer(Direction::NORTH));
        REQUIRE(map.GetPlayer().GetPosition() == Point{0, 0});
    }
}

TEST_CASE("Map::Generate spawns NPCs", "[Map]") {
    Map map(40, 20);
    map.Generate();

    REQUIRE_FALSE(map.GetNpcs().empty());

    const Point playerPos = map.GetPlayer().GetPosition();
    for (const auto& npc : map.GetNpcs()) {
        const Point npcPos = npc.GetPosition();
        REQUIRE(map.IsInsideMap(npcPos.x, npcPos.y));
        REQUIRE(npcPos != playerPos);
    }
}

TEST_CASE("Monsters return to home after losing player", "[Map]") {
    Map map(40, 40);
    map.GetMonsters().clear();
    map.GetChests().clear();
    map.GetNpcs().clear();

    map.GetPlayer().SetPosition({8, 5});
    map.GetMonsters().push_back(Monster({5, 5}, "Goblin", 50, 8, 12, 6));

    EventBus eventBus;
    map.ProcessMonstersTurn(eventBus);

    REQUIRE(map.GetMonsters()[0].IsReturningHome());

    map.GetPlayer().SetPosition({35, 35});
    bool reachedHome = false;
    for (int i = 0; i < 20; ++i) {
        map.ProcessMonstersTurn(eventBus);
        if (map.GetMonsters()[0].GetPosition() == Point{5, 5}) {
            reachedHome = true;
            break;
        }
    }

    REQUIRE(reachedHome);
}

TEST_CASE("Monsters can return home around obstacles", "[Map]") {
    Map map(20, 20);
    map.GetMonsters().clear();
    map.GetChests().clear();
    map.GetNpcs().clear();
    map.GetObstacles().clear();

    map.GetPlayer().SetPosition({19, 19});

    // Vertical wall blocks straight line to home but leaves a gap at y=8.
    for (int y = 2; y <= 7; ++y) {
        map.GetObstacles().push_back(Obstacle(static_cast<int>(Obstacle::Type::ROCK), {9, y}));
    }

    Monster monster({5, 5}, "Goblin", 50, 8, 12, 4);
    monster.SetPosition({12, 5});
    monster.SetReturningHome(true);
    map.GetMonsters().push_back(monster);

    EventBus eventBus;
    bool reachedHome = false;
    for (int i = 0; i < 40; ++i) {
        map.ProcessMonstersTurn(eventBus);
        if (map.GetMonsters()[0].GetPosition() == Point{5, 5}) {
            reachedHome = true;
            break;
        }
    }

    REQUIRE(reachedHome);
}

TEST_CASE("Monsters can chase player around obstacles", "[Map]") {
    Map map(20, 20);
    map.GetMonsters().clear();
    map.GetChests().clear();
    map.GetNpcs().clear();
    map.GetObstacles().clear();

    map.GetPlayer().SetPosition({6, 5});

    // Wall blocks direct path; monster should use gap at y=8.
    for (int y = 2; y <= 7; ++y) {
        map.GetObstacles().push_back(Obstacle(static_cast<int>(Obstacle::Type::ROCK), {9, y}));
    }

    map.GetMonsters().push_back(Monster({12, 5}, "Goblin", 50, 8, 12, 20));

    EventBus eventBus;
    bool reachedAttackRange = false;
    for (int i = 0; i < 40; ++i) {
        map.ProcessMonstersTurn(eventBus);
        Point pos = map.GetMonsters()[0].GetPosition();
        if (std::abs(pos.x - map.GetPlayer().GetPosition().x) <= 1 &&
            std::abs(pos.y - map.GetPlayer().GetPosition().y) <= 1) {
            reachedAttackRange = true;
            break;
        }
    }

    REQUIRE(reachedAttackRange);
}

TEST_CASE("Monster attack publishes combat event payload", "[Map][EventBus]") {
    Map map(20, 20);
    map.GetMonsters().clear();
    map.GetChests().clear();
    map.GetNpcs().clear();
    map.GetObstacles().clear();

    map.GetPlayer().SetPosition({5, 5});
    map.GetMonsters().push_back(Monster({6, 5}, "Goblin", 50, 8, 12, 20));

    EventBus eventBus;
    std::vector<GameEvent> events;
    eventBus.Subscribe([&](const GameEvent& event) {
        events.push_back(event);
    });

    const int hpBefore = map.GetPlayer().GetHealth();
    map.ProcessMonstersTurn(eventBus);
    const int hpAfter = map.GetPlayer().GetHealth();

    REQUIRE(events.size() == 1);
    REQUIRE(events[0].type == GameEventType::Combat);
    REQUIRE(events[0].payload.IsConsistentFor(events[0].type));
    REQUIRE(events[0].payload.combat.attackerName.has_value());
    REQUIRE(events[0].payload.combat.attackerName.value() == "Goblin");
    REQUIRE(events[0].payload.combat.targetName.has_value());
    REQUIRE(events[0].payload.combat.targetName.value() == map.GetPlayer().GetName());
    REQUIRE(events[0].payload.combat.damage.has_value());
    REQUIRE(events[0].payload.combat.damage.value() == hpBefore - hpAfter);
}

TEST_CASE("Map::Save and Map::Load preserve world state", "[Map]") {
    std::remove("map_save.txt");

    Map original(20, 20);
    auto& player = original.GetPlayer();
    player.SetPosition({3, 4});
    player.Hit(35);
    player.PickUpItem(Item("Health Potion", 0, 0, Item::Type::POTION));
    player.PickUpItem(Item("Iron Sword", 0, 9, Item::Type::WEAPON));
    player.UseItem(1); // Equip sword.
    Quest quest(
        1,
        "Hermit",
        "Clear Nearby Threats",
        "Defeat monsters around this area.",
        Quest::Type::KILL_MONSTERS,
        3,
        50
    );
    quest.AddProgress(2);
    player.AddQuest(quest);
    player.GainExperience(130);
    player.AddPlayTimeSeconds(42);

    original.GetMonsters().clear();
    original.GetMonsters().push_back(
        Monster({10, 11}, "Orc", 80, 6, 18, 4, 40, Item("Orc Shield", 8, 0, Item::Type::SHIELD))
    );
    original.GetMonsters()[0].Hit(25);

    original.GetChests().clear();
    Chest openedChest({7, 8}, Item("Quest Scroll", 0, 0, Item::Type::QUEST));
    openedChest.Open();
    original.GetChests().push_back(openedChest);

    original.GetObstacles().clear();
    original.GetObstacles().push_back(Obstacle(static_cast<int>(Obstacle::Type::TREE), {1, 1}));

    original.GetNpcs().clear();
    NPC npc({5, 6}, "Hermit", {"Line one", "Line two"});
    npc.Talk();
    original.GetNpcs().push_back(npc);

    REQUIRE(original.Save());

    Map loaded;
    REQUIRE(loaded.Load());

    REQUIRE(loaded.GetWidth() == original.GetWidth());
    REQUIRE(loaded.GetHeight() == original.GetHeight());

    const auto& loadedPlayer = loaded.GetPlayer();
    const auto& originalPlayer = original.GetPlayer();
    REQUIRE(loadedPlayer.GetPosition() == originalPlayer.GetPosition());
    REQUIRE(loadedPlayer.GetHealth() == originalPlayer.GetHealth());
    REQUIRE(loadedPlayer.GetMaxHealth() == originalPlayer.GetMaxHealth());
    REQUIRE(loadedPlayer.GetLevel() == originalPlayer.GetLevel());
    REQUIRE(loadedPlayer.GetExperience() == originalPlayer.GetExperience());
    REQUIRE(loadedPlayer.GetUnspentStatPoints() == originalPlayer.GetUnspentStatPoints());
    REQUIRE(loadedPlayer.GetPlayTimeSeconds() == originalPlayer.GetPlayTimeSeconds());
    REQUIRE(loadedPlayer.GetInventory() == originalPlayer.GetInventory());
    REQUIRE(loadedPlayer.GetAttack() == originalPlayer.GetAttack());
    REQUIRE(loadedPlayer.GetQuests().size() == originalPlayer.GetQuests().size());
    REQUIRE(loadedPlayer.GetQuests()[0].GetGiver() == originalPlayer.GetQuests()[0].GetGiver());
    REQUIRE(loadedPlayer.GetQuests()[0].GetTitle() == originalPlayer.GetQuests()[0].GetTitle());
    REQUIRE(loadedPlayer.GetQuests()[0].GetCurrentAmount() == originalPlayer.GetQuests()[0].GetCurrentAmount());
    REQUIRE(loadedPlayer.GetQuests()[0].GetTargetAmount() == originalPlayer.GetQuests()[0].GetTargetAmount());

    REQUIRE(loaded.GetMonsters().size() == original.GetMonsters().size());
    REQUIRE(loaded.GetMonsters()[0].GetPosition() == original.GetMonsters()[0].GetPosition());
    REQUIRE(loaded.GetMonsters()[0].GetHealth() == original.GetMonsters()[0].GetHealth());
    REQUIRE(loaded.GetMonsters()[0].GetExperienceReward() == original.GetMonsters()[0].GetExperienceReward());
    REQUIRE(loaded.GetMonsters()[0].GetDropItem() == original.GetMonsters()[0].GetDropItem());

    REQUIRE(loaded.GetChests().size() == original.GetChests().size());
    REQUIRE(loaded.GetChests()[0].GetPosition() == original.GetChests()[0].GetPosition());
    REQUIRE(loaded.GetChests()[0].IsOpen() == original.GetChests()[0].IsOpen());

    REQUIRE(loaded.GetObstacles().size() == original.GetObstacles().size());
    REQUIRE(loaded.GetObstacles()[0].GetPosition() == original.GetObstacles()[0].GetPosition());
    REQUIRE(loaded.GetObstacles()[0].GetType() == original.GetObstacles()[0].GetType());

    REQUIRE(loaded.GetNpcs().size() == original.GetNpcs().size());
    REQUIRE(loaded.GetNpcs()[0].GetPosition() == original.GetNpcs()[0].GetPosition());
    REQUIRE(loaded.GetNpcs()[0].GetName() == original.GetNpcs()[0].GetName());
    REQUIRE(loaded.GetNpcs()[0].GetDialogues() == original.GetNpcs()[0].GetDialogues());
    REQUIRE(loaded.GetNpcs()[0].GetDialogueCursor() == original.GetNpcs()[0].GetDialogueCursor());

    std::remove("map_save.txt");
}
