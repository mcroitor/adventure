#pragma once

#include "Point.hpp"

#include <string>

class Quest {
public:
    enum class Type {
        KILL_MONSTERS = 0,
        COLLECT_ITEMS = 1,
        REACH_POSITION = 2
    };

private:
    int id = 0;
    std::string giver;
    std::string title;
    std::string description;
    Type type = Type::KILL_MONSTERS;
    int targetAmount = 1;
    int currentAmount = 0;
    int rewardExperience = 0;
    bool completed = false;
    bool rewarded = false;
    bool hasTargetPoint = false;
    Point targetPoint{0, 0};

public:
    Quest() = default;

    Quest(
        int questId,
        const std::string& giverName,
        const std::string& questTitle,
        const std::string& questDescription,
        Type questType,
        int questTargetAmount,
        int questRewardExperience,
        bool hasPoint = false,
        Point point = Point{0, 0}
    );

    int GetId() const { return id; }
    const std::string& GetGiver() const { return giver; }
    const std::string& GetTitle() const { return title; }
    const std::string& GetDescription() const { return description; }
    Type GetType() const { return type; }
    int GetTargetAmount() const { return targetAmount; }
    int GetCurrentAmount() const { return currentAmount; }
    int GetRewardExperience() const { return rewardExperience; }
    bool IsCompleted() const { return completed; }
    bool IsRewarded() const { return rewarded; }
    bool HasTargetPoint() const { return hasTargetPoint; }
    Point GetTargetPoint() const { return targetPoint; }

    void AddProgress(int amount);
    void UpdatePosition(const Point& position);
    void SetProgress(int current, bool isCompleted);
    void SetRewarded(bool value);

    std::string ProgressText() const;
};
