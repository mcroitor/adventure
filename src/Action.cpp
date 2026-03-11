#include "Action.hpp"
#include <cctype>

char ActionToChar(Action action){
    return static_cast<char>(action);
}

Action CharToAction(char c) {
    // Convert to uppercase for case-insensitive handling
    c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));

    switch (c) {
    case 'W': return Action::MOVE_NORTH;
    case 'S': return Action::MOVE_SOUTH;
    case 'D': return Action::MOVE_EAST;
    case 'A': return Action::MOVE_WEST;
    case 'I': return Action::PICK_UP_ITEM;
    case 'K': return Action::SHOW_EQUIPMENT;
    case 'F': return Action::ATTACK_MONSTER;
    case '1': return Action::USE_ITEM_1;
    case '2': return Action::USE_ITEM_2;
    case '3': return Action::USE_ITEM_3;
    case '4': return Action::USE_ITEM_4;
    case '5': return Action::USE_ITEM_5;
    case '6': return Action::USE_ITEM_6;
    case '7': return Action::USE_ITEM_7;
    case '8': return Action::USE_ITEM_8;
    case '9': return Action::USE_ITEM_9;
    case 'P': return Action::ALLOCATE_STATS;
    case 'E': return Action::INTERACT;
    case 'M': return Action::OPEN_ACTION_MENU;
    case 'Z': return Action::DRAW_ASCII;
    case 'R': return Action::RESTART_GAME;
    case 'H': return Action::TOGGLE_HELP;
    case 'Q': return Action::QUIT_GAME;
    default: return Action::NONE;
    }
}

std::ostream& operator<<(std::ostream& os, const Action& action) {
    os << ActionToChar(action);
    return os;
}