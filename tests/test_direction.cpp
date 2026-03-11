/**
 * @file test_direction.cpp
 * @brief Unit tests for Direction enum
 */

#include <catch2/catch_test_macros.hpp>
#include "Direction.hpp"

TEST_CASE("Direction enum has all expected values", "[Direction]") {
    SECTION("Cardinal directions exist") {
        Direction north = Direction::NORTH;
        Direction south = Direction::SOUTH;
        Direction east = Direction::EAST;
        Direction west = Direction::WEST;
        
        // Just verify they compile and are distinct
        REQUIRE(north != south);
        REQUIRE(east != west);
        REQUIRE(north != east);
    }
    
    SECTION("Diagonal directions exist") {
        Direction ne = Direction::NORTH_EAST;
        Direction nw = Direction::NORTH_WEST;
        Direction se = Direction::SOUTH_EAST;
        Direction sw = Direction::SOUTH_WEST;
        
        // Verify they are distinct
        REQUIRE(ne != nw);
        REQUIRE(se != sw);
        REQUIRE(ne != se);
    }
    
    SECTION("All 8 directions are distinct") {
        std::vector<Direction> directions = {
            Direction::NORTH,
            Direction::SOUTH,
            Direction::EAST,
            Direction::WEST,
            Direction::NORTH_EAST,
            Direction::NORTH_WEST,
            Direction::SOUTH_EAST,
            Direction::SOUTH_WEST
        };
        
        // Check that all are unique
        for (size_t i = 0; i < directions.size(); i++) {
            for (size_t j = i + 1; j < directions.size(); j++) {
                REQUIRE(directions[i] != directions[j]);
            }
        }
    }
}

TEST_CASE("to_string converts Direction to string", "[Direction]") {
    SECTION("Cardinal directions") {
        REQUIRE(to_string(Direction::NORTH) == "NORTH");
        REQUIRE(to_string(Direction::SOUTH) == "SOUTH");
        REQUIRE(to_string(Direction::EAST) == "EAST");
        REQUIRE(to_string(Direction::WEST) == "WEST");
    }
    
    SECTION("Diagonal directions") {
        REQUIRE(to_string(Direction::NORTH_EAST) == "NORTH_EAST");
        REQUIRE(to_string(Direction::NORTH_WEST) == "NORTH_WEST");
        REQUIRE(to_string(Direction::SOUTH_EAST) == "SOUTH_EAST");
        REQUIRE(to_string(Direction::SOUTH_WEST) == "SOUTH_WEST");
    }
    
    SECTION("String representations are correct length") {
        REQUIRE(to_string(Direction::NORTH).length() == 5);
        REQUIRE(to_string(Direction::EAST).length() == 4);
        REQUIRE(to_string(Direction::NORTH_EAST).length() == 10);
    }
    
    SECTION("String representations are uppercase") {
        std::string north_str = to_string(Direction::NORTH);
        for (char c : north_str) {
            if (c != '_') {
                REQUIRE(std::isupper(static_cast<unsigned char>(c)));
            }
        }
    }
}

TEST_CASE("Direction enum ordering", "[Direction]") {
    SECTION("Cardinal directions have specific ordering") {
        // Test implicit conversion to underlying type
        REQUIRE(static_cast<int>(Direction::NORTH) < static_cast<int>(Direction::SOUTH));
        REQUIRE(static_cast<int>(Direction::NORTH) < static_cast<int>(Direction::EAST));
    }
    
    SECTION("All directions can be compared") {
        Direction d1 = Direction::NORTH;
        Direction d2 = Direction::NORTH;
        Direction d3 = Direction::SOUTH;
        
        REQUIRE(d1 == d2);
        REQUIRE(d1 != d3);
    }
}

TEST_CASE("Direction string conversion is consistent", "[Direction]") {
    SECTION("Converting same direction multiple times gives same result") {
        std::string first = to_string(Direction::NORTH);
        std::string second = to_string(Direction::NORTH);
        REQUIRE(first == second);
    }
    
    SECTION("Different directions give different strings") {
        std::string north = to_string(Direction::NORTH);
        std::string south = to_string(Direction::SOUTH);
        std::string east = to_string(Direction::EAST);
        std::string west = to_string(Direction::WEST);
        
        REQUIRE(north != south);
        REQUIRE(north != east);
        REQUIRE(north != west);
        REQUIRE(south != east);
        REQUIRE(south != west);
        REQUIRE(east != west);
    }
}

TEST_CASE("Direction logical groupings", "[Direction]") {
    SECTION("North-related directions") {
        std::string north = to_string(Direction::NORTH);
        std::string north_east = to_string(Direction::NORTH_EAST);
        std::string north_west = to_string(Direction::NORTH_WEST);
        
        // All contain "NORTH"
        REQUIRE(north.find("NORTH") != std::string::npos);
        REQUIRE(north_east.find("NORTH") != std::string::npos);
        REQUIRE(north_west.find("NORTH") != std::string::npos);
    }
    
    SECTION("South-related directions") {
        std::string south = to_string(Direction::SOUTH);
        std::string south_east = to_string(Direction::SOUTH_EAST);
        std::string south_west = to_string(Direction::SOUTH_WEST);
        
        // All contain "SOUTH"
        REQUIRE(south.find("SOUTH") != std::string::npos);
        REQUIRE(south_east.find("SOUTH") != std::string::npos);
        REQUIRE(south_west.find("SOUTH") != std::string::npos);
    }
    
    SECTION("East-related directions") {
        std::string east = to_string(Direction::EAST);
        std::string north_east = to_string(Direction::NORTH_EAST);
        std::string south_east = to_string(Direction::SOUTH_EAST);
        
        // All contain "EAST"
        REQUIRE(east.find("EAST") != std::string::npos);
        REQUIRE(north_east.find("EAST") != std::string::npos);
        REQUIRE(south_east.find("EAST") != std::string::npos);
    }
    
    SECTION("West-related directions") {
        std::string west = to_string(Direction::WEST);
        std::string north_west = to_string(Direction::NORTH_WEST);
        std::string south_west = to_string(Direction::SOUTH_WEST);
        
        // All contain "WEST"
        REQUIRE(west.find("WEST") != std::string::npos);
        REQUIRE(north_west.find("WEST") != std::string::npos);
        REQUIRE(south_west.find("WEST") != std::string::npos);
    }
}

TEST_CASE("Direction can be used in switch statements", "[Direction]") {
    Direction dir = Direction::NORTH;
    std::string result;
    
    switch(dir) {
        case Direction::NORTH:
            result = "Going North";
            break;
        case Direction::SOUTH:
            result = "Going South";
            break;
        case Direction::EAST:
            result = "Going East";
            break;
        case Direction::WEST:
            result = "Going West";
            break;
        case Direction::NORTH_EAST:
            result = "Going North-East";
            break;
        case Direction::NORTH_WEST:
            result = "Going North-West";
            break;
        case Direction::SOUTH_EAST:
            result = "Going South-East";
            break;
        case Direction::SOUTH_WEST:
            result = "Going South-West";
            break;
    }
    
    REQUIRE(result == "Going North");
}

TEST_CASE("Direction enum can be stored in containers", "[Direction]") {
    SECTION("Vector of directions") {
        std::vector<Direction> path = {
            Direction::NORTH,
            Direction::EAST,
            Direction::SOUTH,
            Direction::WEST
        };
        
        REQUIRE(path.size() == 4);
        REQUIRE(path[0] == Direction::NORTH);
        REQUIRE(path[1] == Direction::EAST);
        REQUIRE(path[2] == Direction::SOUTH);
        REQUIRE(path[3] == Direction::WEST);
    }
    
    SECTION("Array of directions") {
        Direction directions[] = {
            Direction::NORTH,
            Direction::SOUTH,
            Direction::EAST,
            Direction::WEST
        };
        
        REQUIRE(directions[0] == Direction::NORTH);
        REQUIRE(directions[1] == Direction::SOUTH);
    }
}

TEST_CASE("Direction opposite directions", "[Direction]") {
    SECTION("Cardinal opposites are distinct") {
        REQUIRE(Direction::NORTH != Direction::SOUTH);
        REQUIRE(Direction::EAST != Direction::WEST);
    }
    
    SECTION("Diagonal opposites are distinct") {
        REQUIRE(Direction::NORTH_EAST != Direction::SOUTH_WEST);
        REQUIRE(Direction::NORTH_WEST != Direction::SOUTH_EAST);
    }
}

TEST_CASE("Direction string conversion edge cases", "[Direction]") {
    SECTION("String does not contain spaces") {
        for (auto dir : {Direction::NORTH, Direction::SOUTH, Direction::EAST, Direction::WEST,
                        Direction::NORTH_EAST, Direction::NORTH_WEST, 
                        Direction::SOUTH_EAST, Direction::SOUTH_WEST}) {
            std::string str = to_string(dir);
            REQUIRE(str.find(' ') == std::string::npos);
        }
    }
    
    SECTION("String is not empty") {
        for (auto dir : {Direction::NORTH, Direction::SOUTH, Direction::EAST, Direction::WEST,
                        Direction::NORTH_EAST, Direction::NORTH_WEST, 
                        Direction::SOUTH_EAST, Direction::SOUTH_WEST}) {
            std::string str = to_string(dir);
            REQUIRE_FALSE(str.empty());
        }
    }
    
    SECTION("Diagonal directions contain underscore") {
        REQUIRE(to_string(Direction::NORTH_EAST).find('_') != std::string::npos);
        REQUIRE(to_string(Direction::NORTH_WEST).find('_') != std::string::npos);
        REQUIRE(to_string(Direction::SOUTH_EAST).find('_') != std::string::npos);
        REQUIRE(to_string(Direction::SOUTH_WEST).find('_') != std::string::npos);
    }
    
    SECTION("Cardinal directions do not contain underscore") {
        REQUIRE(to_string(Direction::NORTH).find('_') == std::string::npos);
        REQUIRE(to_string(Direction::SOUTH).find('_') == std::string::npos);
        REQUIRE(to_string(Direction::EAST).find('_') == std::string::npos);
        REQUIRE(to_string(Direction::WEST).find('_') == std::string::npos);
    }
}

TEST_CASE("Direction assignment and copying", "[Direction]") {
    SECTION("Direction can be assigned") {
        Direction dir1 = Direction::NORTH;
        Direction dir2 = Direction::SOUTH;
        
        dir2 = dir1;
        REQUIRE(dir2 == Direction::NORTH);
    }
    
    SECTION("Direction can be copied") {
        Direction original = Direction::EAST;
        Direction copy = original;
        
        REQUIRE(copy == original);
        REQUIRE(copy == Direction::EAST);
    }
}

TEST_CASE("DirectionToDelta maps directions to correct coordinate offsets", "[Direction]") {
    SECTION("Cardinal directions") {
        REQUIRE(DirectionToDelta(Direction::NORTH) == Point{0, -1});
        REQUIRE(DirectionToDelta(Direction::SOUTH) == Point{0, 1});
        REQUIRE(DirectionToDelta(Direction::EAST) == Point{1, 0});
        REQUIRE(DirectionToDelta(Direction::WEST) == Point{-1, 0});
    }

    SECTION("Diagonal directions") {
        REQUIRE(DirectionToDelta(Direction::NORTH_EAST) == Point{1, -1});
        REQUIRE(DirectionToDelta(Direction::NORTH_WEST) == Point{-1, -1});
        REQUIRE(DirectionToDelta(Direction::SOUTH_EAST) == Point{1, 1});
        REQUIRE(DirectionToDelta(Direction::SOUTH_WEST) == Point{-1, 1});
    }
}
