/**
 * @file test_player.cpp
 * @brief Unit tests for Player combat and inventory behavior
 */

#include <catch2/catch_test_macros.hpp>

#include "Player.hpp"
#include "Monster.hpp"
#include "Item.hpp"
#include "Map.hpp"

TEST_CASE("Player inventory respects capacity", "[Player]") {
    Player player({0, 0}, "Hero");
    Item potion("Potion", 0, 0, Item::Type::POTION);

    for (int i = 0; i < player.GetMaxInventorySize(); ++i) {
        player.PickUpItem(potion);
    }

    REQUIRE(player.GetInventorySize() == player.GetMaxInventorySize());

    player.PickUpItem(Item("Extra", 0, 0, Item::Type::QUEST));
    REQUIRE(player.GetInventorySize() == player.GetMaxInventorySize());
}

TEST_CASE("Player attack damages monster", "[Player]") {
    Player player({0, 0}, "Hero");
    Monster monster({1, 0}, "Goblin", 50, 5, 5, 3);

    int before = monster.GetHealth();
    int after = player.Attack(monster);

    REQUIRE(after < before);
    REQUIRE(monster.GetHealth() == after);
}

TEST_CASE("Player attack does not affect dead target", "[Player]") {
    Player player({0, 0}, "Hero");
    Monster monster({1, 0}, "Goblin", 10, 5, 5, 3);

    monster.Hit(1000);
    REQUIRE_FALSE(monster.IsAlive());

    int after = player.Attack(monster);
    REQUIRE(after == 0);
    REQUIRE(monster.GetHealth() == 0);
}

TEST_CASE("Potion healing is clamped by max health", "[Player]") {
    Player player({0, 0}, "Hero");
    player.Hit(50);

    REQUIRE(player.GetHealth() == 50);

    player.PickUpItem(Item("Potion", 0, 0, Item::Type::POTION));
    player.UseItem(0);

    REQUIRE(player.GetHealth() == 70);

    player.PickUpItem(Item("Potion", 0, 0, Item::Type::POTION));
    player.UseItem(0);

    REQUIRE(player.GetHealth() == 90);

    player.PickUpItem(Item("Potion", 0, 0, Item::Type::POTION));
    player.UseItem(0);

    REQUIRE(player.GetHealth() == player.GetMaxHealth());
}

TEST_CASE("Direct healing is clamped and cannot revive dead player", "[Player]") {
    Player player({0, 0}, "Hero");

    player.Hit(30);
    REQUIRE(player.GetHealth() == 70);

    REQUIRE(player.Heal(20) == 90);
    REQUIRE(player.Heal(50) == player.GetMaxHealth());
    REQUIRE(player.GetHealth() == player.GetMaxHealth());

    player.Hit(1000);
    REQUIRE_FALSE(player.IsAlive());
    REQUIRE(player.Heal(10) == 0);
    REQUIRE(player.GetHealth() == 0);
}

TEST_CASE("Player gains experience and levels up", "[Player]") {
    Player player({0, 0}, "Hero");

    REQUIRE(player.GetLevel() == 1);
    REQUIRE(player.GetExperience() == 0);
    REQUIRE(player.GetExperienceToNextLevel() == 100);

    player.GainExperience(50);
    REQUIRE(player.GetLevel() == 1);
    REQUIRE(player.GetExperience() == 50);

    player.Hit(80);
    REQUIRE(player.GetHealth() == 20);

    player.GainExperience(60);
    REQUIRE(player.GetLevel() == 2);
    REQUIRE(player.GetExperience() == 10);
    REQUIRE(player.GetExperienceToNextLevel() == 150);
    REQUIRE(player.GetUnspentStatPoints() == 3);
}

TEST_CASE("Player can allocate stat points after level-up", "[Player]") {
    Player player({0, 0}, "Hero");
    player.GainExperience(100);

    REQUIRE(player.GetLevel() == 2);
    REQUIRE(player.GetUnspentStatPoints() == 3);

    int baseStrength = player.GetStrength();
    int baseAgility = player.GetAgility();
    int baseMaxHealth = player.GetMaxHealth();

    REQUIRE(player.AllocateStatPoint(Player::StatType::STRENGTH));
    REQUIRE(player.GetStrength() == baseStrength + 1);

    REQUIRE(player.AllocateStatPoint(Player::StatType::AGILITY));
    REQUIRE(player.GetAgility() == baseAgility + 1);

    REQUIRE(player.AllocateStatPoint(Player::StatType::VITALITY));
    REQUIRE(player.GetMaxHealth() == baseMaxHealth + 5);

    REQUIRE(player.GetUnspentStatPoints() == 0);
    REQUIRE_FALSE(player.AllocateStatPoint(Player::StatType::STRENGTH));
}

TEST_CASE("Monster provides reward and loot", "[Player]") {
    Monster monster(
        {1, 1},
        "Goblin",
        40,
        8,
        10,
        3,
        35,
        Item("Goblin Dagger", 0, 6, Item::Type::WEAPON)
    );

    REQUIRE(monster.GetExperienceReward() == 35);
    REQUIRE(monster.GetDropItem() == Item("Goblin Dagger", 0, 6, Item::Type::WEAPON));
}

TEST_CASE("Player statistics are tracked", "[Player]") {
    Player player({0, 0}, "Hero");

    SECTION("Collected items counter increments only on successful pickup") {
        Item quest("Quest", 0, 0, Item::Type::QUEST);
        for (int i = 0; i < player.GetMaxInventorySize(); ++i) {
            player.PickUpItem(quest);
        }

        REQUIRE(player.GetItemsCollected() == player.GetMaxInventorySize());
        player.PickUpItem(quest);
        REQUIRE(player.GetItemsCollected() == player.GetMaxInventorySize());
    }

    SECTION("Monster kill counter increments on lethal attack") {
        Monster monster({1, 0}, "Goblin", 5, 5, 5, 3);
        REQUIRE(player.GetMonstersKilled() == 0);
        player.Attack(monster);
        REQUIRE(player.GetMonstersKilled() == 1);

        // Re-attacking dead monsters should not increase kill count.
        player.Attack(monster);
        REQUIRE(player.GetMonstersKilled() == 1);
    }

    SECTION("Distance travelled increments through map movement") {
        Map map(20, 20);
        map.GetPlayer().SetPosition({5, 5});

        REQUIRE(map.GetPlayer().GetDistanceTravelled() == 0);
        REQUIRE(map.TryMovePlayer(Direction::EAST));
        REQUIRE(map.GetPlayer().GetDistanceTravelled() == 1);
    }

    SECTION("Play time accumulates positive values only") {
        REQUIRE(player.GetPlayTimeSeconds() == 0);

        player.AddPlayTimeSeconds(5);
        REQUIRE(player.GetPlayTimeSeconds() == 5);

        player.AddPlayTimeSeconds(0);
        player.AddPlayTimeSeconds(-3);
        REQUIRE(player.GetPlayTimeSeconds() == 5);
    }
}
