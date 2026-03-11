/**
 * @file test_chest.cpp
 * @brief Unit tests for Chest class
 */

#include <catch2/catch_test_macros.hpp>
#include "Chest.hpp"
#include "Item.hpp"
#include "Point.hpp"

TEST_CASE("Chest default constructor", "[Chest]") {
    Chest chest;
    
    SECTION("Default chest is closed") {
        REQUIRE(chest.IsOpen() == false);
    }
    
    SECTION("Default chest position is (0,0)") {
        Point pos = chest.GetPosition();
        REQUIRE(pos.x == 0);
        REQUIRE(pos.y == 0);
    }
    
    SECTION("Default chest contains NO_ITEM") {
        Item item = chest.GetItem();
        REQUIRE(item == NO_ITEM);
    }
    
    SECTION("Default chest is not passable") {
        REQUIRE(chest.IsPassable() == false);
    }
    
    SECTION("Default chest symbol is 'C'") {
        REQUIRE(chest.GetSymbol() == 'C');
    }
}

TEST_CASE("Chest constructor with position and item", "[Chest]") {
    Point pos{10, 20};
    Item sword("Sword", 0, 15, Item::Type::WEAPON);
    Chest chest(pos, sword);
    
    SECTION("Chest is at specified position") {
        Point chestPos = chest.GetPosition();
        REQUIRE(chestPos.x == 10);
        REQUIRE(chestPos.y == 20);
    }
    
    SECTION("Chest contains specified item") {
        Item item = chest.GetItem();
        REQUIRE(item.GetName() == "Sword");
        REQUIRE(item.GetAttack() == 15);
        REQUIRE(item.GetType() == Item::Type::WEAPON);
    }
    
    SECTION("Chest is initially closed") {
        REQUIRE(chest.IsOpen() == false);
    }
    
    SECTION("Closed chest is not passable") {
        REQUIRE(chest.IsPassable() == false);
    }
    
    SECTION("Closed chest symbol is 'C'") {
        REQUIRE(chest.GetSymbol() == 'C');
    }
}

TEST_CASE("Chest opening behavior", "[Chest]") {
    Point pos{5, 5};
    Item armor("Plate Armor", 20, 0, Item::Type::ARMOR);
    Chest chest(pos, armor);
    
    SECTION("Opening chest returns the item") {
        Item retrievedItem = chest.Open();
        
        REQUIRE(retrievedItem.GetName() == "Plate Armor");
        REQUIRE(retrievedItem.GetDefense() == 20);
        REQUIRE(retrievedItem.GetType() == Item::Type::ARMOR);
    }
    
    SECTION("After opening, chest is marked as open") {
        chest.Open();
        REQUIRE(chest.IsOpen() == true);
    }
    
    SECTION("After opening, chest becomes passable") {
        REQUIRE(chest.IsPassable() == false);
        chest.Open();
        REQUIRE(chest.IsPassable() == true);
    }
    
    SECTION("After opening, chest symbol changes to 'O'") {
        REQUIRE(chest.GetSymbol() == 'C');
        chest.Open();
        REQUIRE(chest.GetSymbol() == 'O');
    }
    
    SECTION("After opening, chest contains NO_ITEM") {
        chest.Open();
        Item item = chest.GetItem();
        REQUIRE(item == NO_ITEM);
    }
}

TEST_CASE("Opening chest multiple times", "[Chest]") {
    Point pos{3, 7};
    Item potion("Health Potion", 0, 0, Item::Type::POTION);
    Chest chest(pos, potion);
    
    SECTION("First open returns the item") {
        Item item1 = chest.Open();
        REQUIRE(item1.GetName() == "Health Potion");
    }
    
    SECTION("Second open returns NO_ITEM") {
        chest.Open();
        Item item2 = chest.Open();
        REQUIRE(item2 == NO_ITEM);
    }
    
    SECTION("Chest remains open") {
        chest.Open();
        chest.Open();
        REQUIRE(chest.IsOpen() == true);
    }
}

TEST_CASE("Chest copy constructor", "[Chest]") {
    Point pos{15, 25};
    Item shield("Iron Shield", 10, 0, Item::Type::SHIELD);
    Chest original(pos, shield);
    
    Chest copy(original);
    
    SECTION("Copied chest has same position") {
        Point copyPos = copy.GetPosition();
        REQUIRE(copyPos.x == 15);
        REQUIRE(copyPos.y == 25);
    }
    
    SECTION("Copied chest has same item") {
        Item item = copy.GetItem();
        REQUIRE(item.GetName() == "Iron Shield");
        REQUIRE(item.GetDefense() == 10);
    }
    
    SECTION("Copied chest has same open state") {
        REQUIRE(copy.IsOpen() == original.IsOpen());
    }
}

TEST_CASE("Chest assignment operator", "[Chest]") {
    Point pos1{1, 2};
    Item item1("Helmet", 5, 0, Item::Type::HELM);
    Chest chest1(pos1, item1);
    
    Point pos2{10, 20};
    Item item2("Boots", 3, 0, Item::Type::BOOTS);
    Chest chest2(pos2, item2);
    
    chest1 = chest2;
    
    SECTION("Assigned chest has same position") {
        Point chest1Pos = chest1.GetPosition();
        Point chest2Pos = chest2.GetPosition();
        REQUIRE(chest1Pos.x == chest2Pos.x);
        REQUIRE(chest1Pos.y == chest2Pos.y);
    }
    
    SECTION("Assigned chest has same item") {
        Item item = chest1.GetItem();
        REQUIRE(item.GetName() == "Boots");
    }
    
    SECTION("Assigned chest has same open state") {
        REQUIRE(chest1.IsOpen() == chest2.IsOpen());
    }
}

TEST_CASE("Chest equality operator", "[Chest]") {
    Point pos{7, 8};
    Item sword("Sword", 0, 10, Item::Type::WEAPON);
    
    SECTION("Two chests with same properties are equal") {
        Chest chest1(pos, sword);
        Chest chest2(pos, sword);
        REQUIRE(chest1 == chest2);
    }
    
    SECTION("Chests with different positions are not equal") {
        Chest chest1(pos, sword);
        Chest chest2({10, 10}, sword);
        REQUIRE_FALSE(chest1 == chest2);
    }
    
    SECTION("Chests with different items are not equal") {
        Item armor("Armor", 15, 0, Item::Type::ARMOR);
        Chest chest1(pos, sword);
        Chest chest2(pos, armor);
        REQUIRE_FALSE(chest1 == chest2);
    }
    
    SECTION("Chests with different open states are not equal") {
        Chest chest1(pos, sword);
        Chest chest2(pos, sword);
        chest2.Open();
        REQUIRE_FALSE(chest1 == chest2);
    }
}

TEST_CASE("Chest GetSymbol method", "[Chest]") {
    Point pos{0, 0};
    Item item("Item", 0, 0, Item::Type::WEAPON);
    Chest chest(pos, item);
    
    SECTION("Closed chest symbol is 'C'") {
        REQUIRE(chest.GetSymbol() == 'C');
    }
    
    SECTION("Opened chest symbol is 'O'") {
        chest.Open();
        REQUIRE(chest.GetSymbol() == 'O');
    }
    
    SECTION("Symbol changes when opening") {
        char before = chest.GetSymbol();
        chest.Open();
        char after = chest.GetSymbol();
        REQUIRE(before != after);
        REQUIRE(before == 'C');
        REQUIRE(after == 'O');
    }
}

TEST_CASE("Chest passability based on state", "[Chest]") {
    Point pos{5, 10};
    Item item("Item", 0, 0, Item::Type::WEAPON);
    Chest chest(pos, item);
    
    SECTION("Closed chest blocks passage") {
        REQUIRE(chest.IsPassable() == false);
    }
    
    SECTION("Opened chest allows passage") {
        chest.Open();
        REQUIRE(chest.IsPassable() == true);
    }
    
    SECTION("Passability changes when opening") {
        bool beforeOpen = chest.IsPassable();
        chest.Open();
        bool afterOpen = chest.IsPassable();
        
        REQUIRE(beforeOpen == false);
        REQUIRE(afterOpen == true);
    }
}

TEST_CASE("Chest with different item types", "[Chest]") {
    Point pos{0, 0};
    
    SECTION("Chest can contain weapon") {
        Item weapon("Axe", 0, 20, Item::Type::WEAPON);
        Chest chest(pos, weapon);
        Item retrieved = chest.Open();
        REQUIRE(retrieved.GetType() == Item::Type::WEAPON);
    }
    
    SECTION("Chest can contain armor") {
        Item armor("Chain Mail", 15, 0, Item::Type::ARMOR);
        Chest chest(pos, armor);
        Item retrieved = chest.Open();
        REQUIRE(retrieved.GetType() == Item::Type::ARMOR);
    }
    
    SECTION("Chest can contain potion") {
        Item potion("Mana Potion", 0, 0, Item::Type::POTION);
        Chest chest(pos, potion);
        Item retrieved = chest.Open();
        REQUIRE(retrieved.GetType() == Item::Type::POTION);
    }
    
    SECTION("Chest can contain quest item") {
        Item quest("Ancient Key", 0, 0, Item::Type::QUEST);
        Chest chest(pos, quest);
        Item retrieved = chest.Open();
        REQUIRE(retrieved.GetType() == Item::Type::QUEST);
    }
}

TEST_CASE("Chest inherits from MapObject", "[Chest]") {
    Point pos{12, 34};
    Item item("Item", 0, 0, Item::Type::WEAPON);
    Chest chest(pos, item);
    
    SECTION("Chest can be treated as GameObject") {
        GameObject* obj = &chest;
        Point objPos = obj->GetPosition();
        REQUIRE(objPos.x == 12);
        REQUIRE(objPos.y == 34);
    }
    
    SECTION("Chest GetSymbol is virtual") {
        GameObject* obj = &chest;
        REQUIRE(obj->GetSymbol() == 'C');
        chest.Open();
        REQUIRE(obj->GetSymbol() == 'O');
    }
    
    SECTION("Chest IsPassable is virtual") {
        GameObject* obj = &chest;
        REQUIRE(obj->IsPassable() == false);
        chest.Open();
        REQUIRE(obj->IsPassable() == true);
    }
}

TEST_CASE("Empty chest behavior", "[Chest]") {
    Point pos{0, 0};
    Chest chest(pos, NO_ITEM);
    
    SECTION("Empty chest can be opened") {
        Item item = chest.Open();
        REQUIRE(item == NO_ITEM);
        REQUIRE(chest.IsOpen() == true);
    }
    
    SECTION("Empty chest changes symbol when opened") {
        REQUIRE(chest.GetSymbol() == 'C');
        chest.Open();
        REQUIRE(chest.GetSymbol() == 'O');
    }
}
