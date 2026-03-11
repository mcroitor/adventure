#include "Quest.hpp"

#include <algorithm>
#include <cstdlib>
#include <sstream>

Quest::Quest(
    int questId,
    const std::string& giverName,
    const std::string& questTitle,
    const std::string& questDescription,
    Type questType,
    int questTargetAmount,
    int questRewardExperience,
    bool hasPoint,
    Point point
)
    : id(questId),
      giver(giverName),
      title(questTitle),
      description(questDescription),
      type(questType),
      targetAmount(std::max(1, questTargetAmount)),
      currentAmount(0),
      rewardExperience(std::max(0, questRewardExperience)),
      completed(false),
      rewarded(false),
      hasTargetPoint(hasPoint),
      targetPoint(point) {}

void Quest::AddProgress(int amount) {
    if (completed || amount <= 0) {
        return;
    }

    currentAmount += amount;
    if (currentAmount >= targetAmount) {
        currentAmount = targetAmount;
        completed = true;
    }
}

void Quest::UpdatePosition(const Point& position) {
    if (completed || type != Type::REACH_POSITION || !hasTargetPoint) {
        return;
    }

    if (position == targetPoint) {
        currentAmount = targetAmount;
        completed = true;
    }
}

void Quest::SetProgress(int current, bool isCompleted) {
    currentAmount = std::max(0, std::min(current, targetAmount));
    completed = isCompleted || (currentAmount >= targetAmount);
    if (completed) {
        currentAmount = targetAmount;
    }
}

void Quest::SetRewarded(bool value) {
    rewarded = value;
}

std::string Quest::ProgressText() const {
    std::ostringstream os;
    os << title << " [" << currentAmount << "/" << targetAmount << "]";
    if (completed && !rewarded) {
        os << " (ready to turn in)";
    } else if (rewarded) {
        os << " (completed)";
    }
    return os.str();
}
