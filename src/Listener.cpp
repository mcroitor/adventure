#include "Listener.hpp"

#include <curses.h>

int Listener::GetKey(int timeoutMs) {
    timeout(timeoutMs);
    return getch();
}

Action Listener::GetAction(int timeoutMs) {
    const int ch = GetKey(timeoutMs);
    if (ch == ERR) {
        return Action::NONE;
    }

    if (IsToggleHelpInput(ch)) {
        inputBuffer.push_back(Action::TOGGLE_HELP);
        return Action::TOGGLE_HELP;
    }

    if (ch < 0 || ch > 255) {
        return Action::NONE;
    }

    auto action = CharToAction(static_cast<char>(ch));
    if (action != Action::NONE) {
        inputBuffer.push_back(action);
    }
    return action;
}

bool Listener::IsToggleHelpInput(int key) const {
    if (key == KEY_F(1)) {
        return true;
    }

    if (key < 0 || key > 255) {
        return false;
    }

    return CharToAction(static_cast<char>(key)) == Action::TOGGLE_HELP;
}

std::vector<Action> Listener::GetInputBuffer() const {
    return inputBuffer;
}

void Listener::ClearInputBuffer() {
    inputBuffer.clear();
}