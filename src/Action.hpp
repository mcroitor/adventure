/**
 * @file Action.hpp
 * @brief Definition of Action enum and related functions
 * @author Mihail Croitor
 * @date 2025
 * @version 1.0
 */
#pragma once

#include <ostream>

/**
 * @brief Actions that can be performed by the player
 * 
 * Enum representing various player actions such as movement,
 * item usage, attacking, and interaction.
 */
enum class Action: char {
    NONE = '\0',
    MOVE_NORTH = 'W',
    MOVE_SOUTH = 'S',
    MOVE_EAST = 'D',
    MOVE_WEST = 'A',
    PICK_UP_ITEM = 'I',
    SHOW_EQUIPMENT = 'K',
    ATTACK_MONSTER = 'F',
    USE_ITEM_1 = '1',
    USE_ITEM_2 = '2',
    USE_ITEM_3 = '3',
    USE_ITEM_4 = '4',
    USE_ITEM_5 = '5',
    USE_ITEM_6 = '6',
    USE_ITEM_7 = '7',
    USE_ITEM_8 = '8',
    USE_ITEM_9 = '9',
    ALLOCATE_STATS = 'P',
    INTERACT = 'E',
    OPEN_ACTION_MENU = 'M',
    RESTART_GAME = 'R',
    TOGGLE_HELP = 'H',
    QUIT_GAME = 'Q'
};

/**
 * @brief Convert Action enum to corresponding character
 * @param action Action enum value
 * @return Corresponding character
 */
char ActionToChar(Action action);
/**
 * @brief Convert character to corresponding Action enum
 * @param c Character representing action
 * @return Corresponding Action enum value
 */
Action CharToAction(char c);

/**
 * @brief Stream output operator for Action enum
 * @param os Output stream
 * @param action Action enum value
 * @return Reference to output stream
 */
std::ostream& operator<<(std::ostream& os, const Action& action);
