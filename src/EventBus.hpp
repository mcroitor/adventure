#pragma once

#include <functional>
#include <optional>
#include <string>
#include <utility>
#include <vector>

enum class GameEventType {
    System,
    Save,
    Spawn,
    Combat,
    Quest,
    Warning,
    Error
};

struct GameEvent {
    GameEventType type = GameEventType::System;
    std::string message;
    struct Payload {
        struct CombatData {
            std::optional<std::string> attackerName;
            std::optional<std::string> targetName;
            std::optional<int> damage;
            std::optional<int> experienceGained;

            bool HasAny() const {
                return attackerName.has_value() ||
                       targetName.has_value() ||
                       damage.has_value() ||
                       experienceGained.has_value();
            }
        } combat;

        struct QuestData {
            std::optional<int> questId;
            std::optional<std::string> questTitle;
            std::optional<int> progress;
            std::optional<int> rewardExperience;

            bool HasAny() const {
                return questId.has_value() ||
                       questTitle.has_value() ||
                       progress.has_value() ||
                       rewardExperience.has_value();
            }
        } quest;

        struct SpawnData {
            std::optional<std::string> monsterName;
            std::optional<int> remainingRespawns;

            bool HasAny() const {
                return monsterName.has_value() || remainingRespawns.has_value();
            }
        } spawn;

        struct InventoryData {
            std::optional<std::string> itemName;
            std::optional<bool> inventoryFull;

            bool HasAny() const {
                return itemName.has_value() || inventoryFull.has_value();
            }
        } inventory;

        struct ProgressionData {
            std::optional<std::string> actorName;
            std::optional<int> level;
            std::optional<int> unspentStatPoints;

            bool HasAny() const {
                return actorName.has_value() ||
                       level.has_value() ||
                       unspentStatPoints.has_value();
            }
        } progression;

        bool IsEmpty() const {
            return !combat.HasAny() &&
                   !quest.HasAny() &&
                   !spawn.HasAny() &&
                   !inventory.HasAny() &&
                   !progression.HasAny();
        }

        bool IsConsistentFor(GameEventType type) const {
            const bool hasCombat = combat.HasAny();
            const bool hasQuest = quest.HasAny();
            const bool hasSpawn = spawn.HasAny();
            const bool hasInventory = inventory.HasAny();
            const bool hasProgression = progression.HasAny();
            const int sectionCount =
                (hasCombat ? 1 : 0) +
                (hasQuest ? 1 : 0) +
                (hasSpawn ? 1 : 0) +
                (hasInventory ? 1 : 0) +
                (hasProgression ? 1 : 0);

            if (!hasCombat && !hasQuest && !hasSpawn && !hasInventory && !hasProgression) {
                return true;
            }

            switch (type) {
            case GameEventType::Combat:
                return hasCombat && sectionCount == 1;
            case GameEventType::Quest:
                return hasQuest && sectionCount == 1;
            case GameEventType::Spawn:
                return hasSpawn && sectionCount == 1;
            case GameEventType::System:
                return !hasCombat && !hasQuest && !hasSpawn && (hasInventory || hasProgression);
            case GameEventType::Warning:
                return !hasCombat && !hasQuest && !hasSpawn && (hasInventory || hasProgression);
            case GameEventType::Save:
            case GameEventType::Error:
                return false;
            default:
                return false;
            }
        }
    } payload;
};

namespace GameEventFactory {

inline GameEvent Basic(GameEventType type, std::string message) {
    return GameEvent{type, std::move(message), {}};
}

inline GameEvent Combat(
    std::string message,
    std::optional<std::string> attackerName = std::nullopt,
    std::optional<std::string> targetName = std::nullopt,
    std::optional<int> damage = std::nullopt,
    std::optional<int> experienceGained = std::nullopt
) {
    GameEvent::Payload payload;
    payload.combat.attackerName = std::move(attackerName);
    payload.combat.targetName = std::move(targetName);
    payload.combat.damage = damage;
    payload.combat.experienceGained = experienceGained;
    return GameEvent{GameEventType::Combat, std::move(message), std::move(payload)};
}

inline GameEvent Quest(
    std::string message,
    std::optional<int> questId = std::nullopt,
    std::optional<std::string> questTitle = std::nullopt,
    std::optional<int> progress = std::nullopt,
    std::optional<int> rewardExperience = std::nullopt
) {
    GameEvent::Payload payload;
    payload.quest.questId = questId;
    payload.quest.questTitle = std::move(questTitle);
    payload.quest.progress = progress;
    payload.quest.rewardExperience = rewardExperience;
    return GameEvent{GameEventType::Quest, std::move(message), std::move(payload)};
}

inline GameEvent Spawn(
    std::string message,
    std::optional<std::string> monsterName = std::nullopt,
    std::optional<int> remainingRespawns = std::nullopt
) {
    GameEvent::Payload payload;
    payload.spawn.monsterName = std::move(monsterName);
    payload.spawn.remainingRespawns = remainingRespawns;
    return GameEvent{GameEventType::Spawn, std::move(message), std::move(payload)};
}

inline GameEvent Inventory(
    GameEventType type,
    std::string message,
    std::optional<std::string> itemName = std::nullopt,
    std::optional<bool> inventoryFull = std::nullopt
) {
    GameEvent::Payload payload;
    payload.inventory.itemName = std::move(itemName);
    payload.inventory.inventoryFull = inventoryFull;
    return GameEvent{type, std::move(message), std::move(payload)};
}

inline GameEvent Progression(
    GameEventType type,
    std::string message,
    std::optional<std::string> actorName = std::nullopt,
    std::optional<int> level = std::nullopt,
    std::optional<int> unspentStatPoints = std::nullopt
) {
    GameEvent::Payload payload;
    payload.progression.actorName = std::move(actorName);
    payload.progression.level = level;
    payload.progression.unspentStatPoints = unspentStatPoints;
    return GameEvent{type, std::move(message), std::move(payload)};
}

} // namespace GameEventFactory

class EventBus {
public:
    using Handler = std::function<void(const GameEvent&)>;

    int Subscribe(Handler handler);
    void Unsubscribe(int subscriptionId);
    void Publish(const GameEvent& event) const;

private:
    int nextSubscriptionId = 1;
    std::vector<std::pair<int, Handler>> handlers;
};
