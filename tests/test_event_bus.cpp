/**
 * @file test_event_bus.cpp
 * @brief Unit tests for EventBus
 */

#include <catch2/catch_test_macros.hpp>

#include "EventBus.hpp"

#include <string>
#include <vector>

TEST_CASE("EventBus publishes events to subscribers", "[EventBus]") {
    EventBus bus;
    std::vector<GameEvent> received;

    bus.Subscribe([&](const GameEvent& event) {
        received.push_back(event);
    });

    bus.Publish(GameEventFactory::Combat("Hero hits Goblin", "Hero", "Goblin", 12));

    REQUIRE(received.size() == 1);
    REQUIRE(received[0].type == GameEventType::Combat);
    REQUIRE(received[0].message == "Hero hits Goblin");
    REQUIRE(received[0].payload.combat.attackerName.has_value());
    REQUIRE(received[0].payload.combat.attackerName.value() == "Hero");
    REQUIRE(received[0].payload.combat.targetName.has_value());
    REQUIRE(received[0].payload.combat.targetName.value() == "Goblin");
    REQUIRE(received[0].payload.combat.damage.has_value());
    REQUIRE(received[0].payload.combat.damage.value() == 12);
}

TEST_CASE("GameEventFactory creates consistent typed events", "[EventBus]") {
    const GameEvent combat = GameEventFactory::Combat("Hit", "Hero", "Goblin", 7);
    REQUIRE(combat.type == GameEventType::Combat);
    REQUIRE(combat.payload.IsConsistentFor(combat.type));
    REQUIRE(combat.payload.combat.attackerName.value() == "Hero");
    REQUIRE(combat.payload.combat.targetName.value() == "Goblin");
    REQUIRE(combat.payload.combat.damage.value() == 7);

    const GameEvent quest = GameEventFactory::Quest("Quest updated", 11, "Scout Route", 2, 50);
    REQUIRE(quest.type == GameEventType::Quest);
    REQUIRE(quest.payload.IsConsistentFor(quest.type));
    REQUIRE(quest.payload.quest.questId.value() == 11);
    REQUIRE(quest.payload.quest.questTitle.value() == "Scout Route");
    REQUIRE(quest.payload.quest.progress.value() == 2);
    REQUIRE(quest.payload.quest.rewardExperience.value() == 50);

    const GameEvent inventory = GameEventFactory::Inventory(
        GameEventType::Warning,
        "Inventory full",
        "Potion",
        true
    );
    REQUIRE(inventory.type == GameEventType::Warning);
    REQUIRE(inventory.payload.IsConsistentFor(inventory.type));
    REQUIRE(inventory.payload.inventory.itemName.value() == "Potion");
    REQUIRE(inventory.payload.inventory.inventoryFull.value());
}

TEST_CASE("Event payload consistency check detects mismatched sections", "[EventBus]") {
    GameEvent::Payload questPayload;
    questPayload.quest.questId = 100;
    questPayload.quest.questTitle = "Scout Route";

    REQUIRE(questPayload.IsConsistentFor(GameEventType::Quest));
    REQUIRE_FALSE(questPayload.IsConsistentFor(GameEventType::Combat));

    GameEvent::Payload inventoryPayload;
    inventoryPayload.inventory.itemName = "Potion";
    inventoryPayload.inventory.inventoryFull = false;

    REQUIRE(inventoryPayload.IsConsistentFor(GameEventType::System));
    REQUIRE(inventoryPayload.IsConsistentFor(GameEventType::Warning));
    REQUIRE_FALSE(inventoryPayload.IsConsistentFor(GameEventType::Save));

    GameEvent::Payload mixedPayload;
    mixedPayload.combat.damage = 5;
    mixedPayload.inventory.itemName = "Loot";

    REQUIRE_FALSE(mixedPayload.IsConsistentFor(GameEventType::Combat));
}

TEST_CASE("EventBus unsubscribe removes handler", "[EventBus]") {
    EventBus bus;
    int callCount = 0;

    const int id = bus.Subscribe([&](const GameEvent&) {
        ++callCount;
    });

    bus.Publish(GameEvent{GameEventType::System, "before unsubscribe"});
    bus.Unsubscribe(id);
    bus.Publish(GameEvent{GameEventType::System, "after unsubscribe"});

    REQUIRE(callCount == 1);
}

TEST_CASE("EventBus supports multiple subscribers", "[EventBus]") {
    EventBus bus;
    int firstCalls = 0;
    int secondCalls = 0;

    bus.Subscribe([&](const GameEvent&) {
        ++firstCalls;
    });
    bus.Subscribe([&](const GameEvent&) {
        ++secondCalls;
    });

    bus.Publish(GameEvent{GameEventType::Save, "Game auto-saved"});

    REQUIRE(firstCalls == 1);
    REQUIRE(secondCalls == 1);
}
