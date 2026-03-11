#pragma once
#include "Action.hpp"

#include <vector>

class Listener {
    std::vector<Action> inputBuffer;
public:
    int GetKey(int timeoutMs = -1);
    Action GetAction(int timeoutMs = -1);
    bool IsToggleHelpInput(int key) const;
    std::vector<Action> GetInputBuffer() const;
    void ClearInputBuffer();
};