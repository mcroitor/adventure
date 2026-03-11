/**
 * @file test_action.cpp
 * @brief Unit tests for Action enum and related functions
 */

#include <catch2/catch_test_macros.hpp>
#include "Action.hpp"
#include <sstream>

TEST_CASE("Action enum values are correctly defined", "[Action]") {
    SECTION("Movement actions have correct character values") {
        REQUIRE(static_cast<char>(Action::MOVE_NORTH) == 'W');
        REQUIRE(static_cast<char>(Action::MOVE_SOUTH) == 'S');
        REQUIRE(static_cast<char>(Action::MOVE_EAST) == 'D');
        REQUIRE(static_cast<char>(Action::MOVE_WEST) == 'A');
    }
    
    SECTION("Interaction actions have correct character values") {
        REQUIRE(static_cast<char>(Action::PICK_UP_ITEM) == 'I');
        REQUIRE(static_cast<char>(Action::SHOW_EQUIPMENT) == 'K');
        REQUIRE(static_cast<char>(Action::ATTACK_MONSTER) == 'F');
        REQUIRE(static_cast<char>(Action::ALLOCATE_STATS) == 'P');
        REQUIRE(static_cast<char>(Action::INTERACT) == 'E');
        REQUIRE(static_cast<char>(Action::OPEN_ACTION_MENU) == 'M');
        REQUIRE(static_cast<char>(Action::RESTART_GAME) == 'R');
        REQUIRE(static_cast<char>(Action::TOGGLE_HELP) == 'H');
        REQUIRE(static_cast<char>(Action::QUIT_GAME) == 'Q');
    }
    
    SECTION("Item slot actions have correct numeric values") {
        REQUIRE(static_cast<char>(Action::USE_ITEM_1) == '1');
        REQUIRE(static_cast<char>(Action::USE_ITEM_2) == '2');
        REQUIRE(static_cast<char>(Action::USE_ITEM_3) == '3');
        REQUIRE(static_cast<char>(Action::USE_ITEM_4) == '4');
        REQUIRE(static_cast<char>(Action::USE_ITEM_5) == '5');
        REQUIRE(static_cast<char>(Action::USE_ITEM_6) == '6');
        REQUIRE(static_cast<char>(Action::USE_ITEM_7) == '7');
        REQUIRE(static_cast<char>(Action::USE_ITEM_8) == '8');
        REQUIRE(static_cast<char>(Action::USE_ITEM_9) == '9');
    }
}

TEST_CASE("ActionToChar converts Action to character", "[Action]") {
    SECTION("Movement actions") {
        REQUIRE(ActionToChar(Action::MOVE_NORTH) == 'W');
        REQUIRE(ActionToChar(Action::MOVE_SOUTH) == 'S');
        REQUIRE(ActionToChar(Action::MOVE_EAST) == 'D');
        REQUIRE(ActionToChar(Action::MOVE_WEST) == 'A');
    }
    
    SECTION("Interaction actions") {
        REQUIRE(ActionToChar(Action::PICK_UP_ITEM) == 'I');
        REQUIRE(ActionToChar(Action::SHOW_EQUIPMENT) == 'K');
        REQUIRE(ActionToChar(Action::ATTACK_MONSTER) == 'F');
        REQUIRE(ActionToChar(Action::ALLOCATE_STATS) == 'P');
        REQUIRE(ActionToChar(Action::INTERACT) == 'E');
        REQUIRE(ActionToChar(Action::OPEN_ACTION_MENU) == 'M');
        REQUIRE(ActionToChar(Action::RESTART_GAME) == 'R');
        REQUIRE(ActionToChar(Action::TOGGLE_HELP) == 'H');
        REQUIRE(ActionToChar(Action::QUIT_GAME) == 'Q');
    }
    
    SECTION("Item slot actions") {
        REQUIRE(ActionToChar(Action::USE_ITEM_1) == '1');
        REQUIRE(ActionToChar(Action::USE_ITEM_5) == '5');
        REQUIRE(ActionToChar(Action::USE_ITEM_9) == '9');
    }
}

TEST_CASE("CharToAction converts character to Action", "[Action]") {
    SECTION("Movement characters") {
        REQUIRE(CharToAction('W') == Action::MOVE_NORTH);
        REQUIRE(CharToAction('S') == Action::MOVE_SOUTH);
        REQUIRE(CharToAction('D') == Action::MOVE_EAST);
        REQUIRE(CharToAction('A') == Action::MOVE_WEST);
    }
    
    SECTION("Lowercase movement characters") {
        REQUIRE(CharToAction('w') == Action::MOVE_NORTH);
        REQUIRE(CharToAction('s') == Action::MOVE_SOUTH);
        REQUIRE(CharToAction('d') == Action::MOVE_EAST);
        REQUIRE(CharToAction('a') == Action::MOVE_WEST);
    }
    
    SECTION("Interaction characters") {
        REQUIRE(CharToAction('I') == Action::PICK_UP_ITEM);
        REQUIRE(CharToAction('i') == Action::PICK_UP_ITEM);
        REQUIRE(CharToAction('K') == Action::SHOW_EQUIPMENT);
        REQUIRE(CharToAction('k') == Action::SHOW_EQUIPMENT);
        REQUIRE(CharToAction('F') == Action::ATTACK_MONSTER);
        REQUIRE(CharToAction('f') == Action::ATTACK_MONSTER);
        REQUIRE(CharToAction('P') == Action::ALLOCATE_STATS);
        REQUIRE(CharToAction('p') == Action::ALLOCATE_STATS);
        REQUIRE(CharToAction('E') == Action::INTERACT);
        REQUIRE(CharToAction('e') == Action::INTERACT);
        REQUIRE(CharToAction('M') == Action::OPEN_ACTION_MENU);
        REQUIRE(CharToAction('m') == Action::OPEN_ACTION_MENU);
        REQUIRE(CharToAction('R') == Action::RESTART_GAME);
        REQUIRE(CharToAction('r') == Action::RESTART_GAME);
        REQUIRE(CharToAction('H') == Action::TOGGLE_HELP);
        REQUIRE(CharToAction('h') == Action::TOGGLE_HELP);
        REQUIRE(CharToAction('Q') == Action::QUIT_GAME);
        REQUIRE(CharToAction('q') == Action::QUIT_GAME);
    }
    
    SECTION("Numeric characters") {
        REQUIRE(CharToAction('1') == Action::USE_ITEM_1);
        REQUIRE(CharToAction('2') == Action::USE_ITEM_2);
        REQUIRE(CharToAction('3') == Action::USE_ITEM_3);
        REQUIRE(CharToAction('4') == Action::USE_ITEM_4);
        REQUIRE(CharToAction('5') == Action::USE_ITEM_5);
        REQUIRE(CharToAction('6') == Action::USE_ITEM_6);
        REQUIRE(CharToAction('7') == Action::USE_ITEM_7);
        REQUIRE(CharToAction('8') == Action::USE_ITEM_8);
        REQUIRE(CharToAction('9') == Action::USE_ITEM_9);
    }
}

TEST_CASE("Action round-trip conversion", "[Action]") {
    SECTION("Action to char and back") {
        auto testRoundTrip = [](Action action) {
            char c = ActionToChar(action);
            Action result = CharToAction(c);
            REQUIRE(result == action);
        };
        
        testRoundTrip(Action::MOVE_NORTH);
        testRoundTrip(Action::MOVE_SOUTH);
        testRoundTrip(Action::MOVE_EAST);
        testRoundTrip(Action::MOVE_WEST);
        testRoundTrip(Action::PICK_UP_ITEM);
        testRoundTrip(Action::SHOW_EQUIPMENT);
        testRoundTrip(Action::ATTACK_MONSTER);
        testRoundTrip(Action::ALLOCATE_STATS);
        testRoundTrip(Action::INTERACT);
        testRoundTrip(Action::OPEN_ACTION_MENU);
        testRoundTrip(Action::RESTART_GAME);
        testRoundTrip(Action::TOGGLE_HELP);
        testRoundTrip(Action::QUIT_GAME);
        testRoundTrip(Action::USE_ITEM_1);
        testRoundTrip(Action::USE_ITEM_5);
        testRoundTrip(Action::USE_ITEM_9);
    }
}

TEST_CASE("Action stream output operator", "[Action]") {
    SECTION("Action can be output to stream") {
        std::ostringstream oss;
        
        oss << Action::MOVE_NORTH;
        REQUIRE(oss.str() == "W");
        
        oss.str("");
        oss << Action::QUIT_GAME;
        REQUIRE(oss.str() == "Q");
        
        oss.str("");
        oss << Action::USE_ITEM_5;
        REQUIRE(oss.str() == "5");
    }
    
    SECTION("Multiple actions can be output sequentially") {
        std::ostringstream oss;
        oss << Action::MOVE_NORTH << Action::MOVE_EAST << Action::ATTACK_MONSTER;
        REQUIRE(oss.str() == "WDF");
    }
}

TEST_CASE("Case insensitivity for CharToAction", "[Action]") {
    SECTION("Uppercase and lowercase produce same result") {
        REQUIRE(CharToAction('W') == CharToAction('w'));
        REQUIRE(CharToAction('A') == CharToAction('a'));
        REQUIRE(CharToAction('S') == CharToAction('s'));
        REQUIRE(CharToAction('D') == CharToAction('d'));
        REQUIRE(CharToAction('I') == CharToAction('i'));
        REQUIRE(CharToAction('K') == CharToAction('k'));
        REQUIRE(CharToAction('F') == CharToAction('f'));
        REQUIRE(CharToAction('P') == CharToAction('p'));
        REQUIRE(CharToAction('E') == CharToAction('e'));
        REQUIRE(CharToAction('M') == CharToAction('m'));
        REQUIRE(CharToAction('R') == CharToAction('r'));
        REQUIRE(CharToAction('H') == CharToAction('h'));
        REQUIRE(CharToAction('Q') == CharToAction('q'));
    }
}

TEST_CASE("CharToAction returns NONE for unsupported input", "[Action]") {
    REQUIRE(CharToAction('\0') == Action::NONE);
    REQUIRE(CharToAction(' ') == Action::NONE);
    REQUIRE(CharToAction('X') == Action::NONE);
}
