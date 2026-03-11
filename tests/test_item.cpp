/**
 * @file test_item.cpp
 * @brief Unit tests for Item class
 */

#include <catch2/catch_test_macros.hpp>
#include "../src/Item.hpp"
#include <sstream>

TEST_CASE("Item can be created with default constructor", "[Item]") {
    Item item;
    
    REQUIRE(item.GetName() == "");
    REQUIRE(item.GetDefense() == 0);
    REQUIRE(item.GetAttack() == 0);
    REQUIRE(item.GetType() == Item::Type::NONE);
}

TEST_CASE("Item can be created with parameters", "[Item]") {
    Item item("Iron Sword", 0, 15, Item::Type::WEAPON);
    
    REQUIRE(item.GetName() == "Iron Sword");
    REQUIRE(item.GetType() == Item::Type::WEAPON);
    REQUIRE(item.GetDefense() == 0);
    REQUIRE(item.GetAttack() == 15);
}

TEST_CASE("Item type enum has all expected values", "[Item]") {
    REQUIRE(static_cast<int>(Item::Type::NONE) == 0);
    REQUIRE(static_cast<int>(Item::Type::HELM) == 1);
    REQUIRE(static_cast<int>(Item::Type::ARMOR) == 2);
    REQUIRE(static_cast<int>(Item::Type::WEAPON) == 3);
    REQUIRE(static_cast<int>(Item::Type::BOOTS) == 4);
    REQUIRE(static_cast<int>(Item::Type::SHIELD) == 5);
    REQUIRE(static_cast<int>(Item::Type::POTION) == 6);
    REQUIRE(static_cast<int>(Item::Type::QUEST) == 7);
}

TEST_CASE("Item weapon types have attack but no defense", "[Item]") {
    Item sword("Sword", 0, 20, Item::Type::WEAPON);
    
    REQUIRE(sword.GetType() == Item::Type::WEAPON);
    REQUIRE(sword.GetAttack() == 20);
    REQUIRE(sword.GetDefense() == 0);
}

TEST_CASE("Item armor types have defense but no attack", "[Item]") {
    SECTION("Helm") {
        Item helm("Iron Helm", 5, 0, Item::Type::HELM);
        REQUIRE(helm.GetType() == Item::Type::HELM);
        REQUIRE(helm.GetDefense() == 5);
        REQUIRE(helm.GetAttack() == 0);
    }
    
    SECTION("Armor") {
        Item armor("Chainmail", 15, 0, Item::Type::ARMOR);
        REQUIRE(armor.GetType() == Item::Type::ARMOR);
        REQUIRE(armor.GetDefense() == 15);
        REQUIRE(armor.GetAttack() == 0);
    }
    
    SECTION("Boots") {
        Item boots("Leather Boots", 3, 0, Item::Type::BOOTS);
        REQUIRE(boots.GetType() == Item::Type::BOOTS);
        REQUIRE(boots.GetDefense() == 3);
        REQUIRE(boots.GetAttack() == 0);
    }
    
    SECTION("Shield") {
        Item shield("Tower Shield", 10, 0, Item::Type::SHIELD);
        REQUIRE(shield.GetType() == Item::Type::SHIELD);
        REQUIRE(shield.GetDefense() == 10);
        REQUIRE(shield.GetAttack() == 0);
    }
}

TEST_CASE("Item potion has no combat stats", "[Item]") {
    Item potion("Health Potion", 0, 0, Item::Type::POTION);
    
    REQUIRE(potion.GetType() == Item::Type::POTION);
    REQUIRE(potion.GetDefense() == 0);
    REQUIRE(potion.GetAttack() == 0);
    REQUIRE(potion.GetName() == "Health Potion");
}

TEST_CASE("Item quest item has no combat stats", "[Item]") {
    Item quest("Ancient Key", 0, 0, Item::Type::QUEST);
    
    REQUIRE(quest.GetType() == Item::Type::QUEST);
    REQUIRE(quest.GetDefense() == 0);
    REQUIRE(quest.GetAttack() == 0);
    REQUIRE(quest.GetName() == "Ancient Key");
}

TEST_CASE("Item can have both attack and defense", "[Item]") {
    Item magic("Magic Staff", 5, 12, Item::Type::WEAPON);
    
    REQUIRE(magic.GetAttack() == 12);
    REQUIRE(magic.GetDefense() == 5);
}

TEST_CASE("Item with empty name", "[Item]") {
    Item empty("", 5, 0, Item::Type::HELM);
    
    REQUIRE(empty.GetName() == "");
    REQUIRE(empty.GetDefense() == 5);
}

TEST_CASE("Item copy constructor", "[Item]") {
    Item original("Excalibur", 0, 50, Item::Type::WEAPON);
    Item copy(original);
    
    REQUIRE(copy.GetName() == original.GetName());
    REQUIRE(copy.GetType() == original.GetType());
    REQUIRE(copy.GetDefense() == original.GetDefense());
    REQUIRE(copy.GetAttack() == original.GetAttack());
}

TEST_CASE("Item assignment operator", "[Item]") {
    Item item1("Sword", 0, 10, Item::Type::WEAPON);
    Item item2;
    
    item2 = item1;
    
    REQUIRE(item2.GetName() == "Sword");
    REQUIRE(item2.GetType() == Item::Type::WEAPON);
    REQUIRE(item2.GetAttack() == 10);
}

TEST_CASE("Item equality comparison", "[Item]") {
    Item item1("Sword", 0, 10, Item::Type::WEAPON);
    Item item2("Sword", 0, 10, Item::Type::WEAPON);
    Item item3("Axe", 0, 12, Item::Type::WEAPON);
    
    REQUIRE(item1 == item2);
    REQUIRE_FALSE(item1 == item3);
}

TEST_CASE("Item stream output operator", "[Item]") {
    Item item("Iron Sword", 0, 15, Item::Type::WEAPON);
    std::ostringstream oss;
    
    oss << item;
    std::string output = oss.str();
    
    REQUIRE(output.find("Iron Sword") != std::string::npos);
}

TEST_CASE("Item with maximum stats", "[Item]") {
    Item ultimate("Ultimate Item", 9999, 9999, Item::Type::WEAPON);
    
    REQUIRE(ultimate.GetDefense() == 9999);
    REQUIRE(ultimate.GetAttack() == 9999);
}

TEST_CASE("Item type categorization", "[Item]") {
    SECTION("Equipment items") {
        Item helm("Helm", 5, 0, Item::Type::HELM);
        Item armor("Armor", 10, 0, Item::Type::ARMOR);
        Item weapon("Weapon", 0, 10, Item::Type::WEAPON);
        Item boots("Boots", 3, 0, Item::Type::BOOTS);
        Item shield("Shield", 8, 0, Item::Type::SHIELD);
        
        REQUIRE((helm.GetType() == Item::Type::HELM ||
                 helm.GetType() == Item::Type::ARMOR ||
                 helm.GetType() == Item::Type::WEAPON ||
                 helm.GetType() == Item::Type::BOOTS ||
                 helm.GetType() == Item::Type::SHIELD));
    }
    
    SECTION("Non-equipment items") {
        Item potion("Potion", 0, 0, Item::Type::POTION);
        Item quest("Quest", 0, 0, Item::Type::QUEST);
        Item nothing;
        
        REQUIRE((potion.GetType() == Item::Type::POTION ||
                 quest.GetType() == Item::Type::QUEST ||
                 nothing.GetType() == Item::Type::NONE));
    }
}

TEST_CASE("Item complete set", "[Item]") {
    Item helm("Steel Helm", 8, 0, Item::Type::HELM);
    Item armor("Steel Plate", 20, 0, Item::Type::ARMOR);
    Item weapon("Steel Sword", 0, 25, Item::Type::WEAPON);
    Item boots("Steel Boots", 5, 0, Item::Type::BOOTS);
    Item shield("Steel Shield", 12, 0, Item::Type::SHIELD);
    
    int totalDefense = helm.GetDefense() + armor.GetDefense() + 
                       boots.GetDefense() + shield.GetDefense();
    int totalAttack = weapon.GetAttack();
    
    REQUIRE(totalDefense == 45);
    REQUIRE(totalAttack == 25);
}

TEST_CASE("Item NONE type represents empty slot", "[Item]") {
    Item empty;
    
    REQUIRE(empty.GetType() == Item::Type::NONE);
    REQUIRE(empty.GetName() == "");
    REQUIRE(empty.GetDefense() == 0);
    REQUIRE(empty.GetAttack() == 0);
}

TEST_CASE("NO_ITEM constant", "[Item]") {
    REQUIRE(NO_ITEM.GetType() == Item::Type::NONE);
    REQUIRE(NO_ITEM.GetName() == "no item");
    REQUIRE(NO_ITEM.GetDefense() == 0);
    REQUIRE(NO_ITEM.GetAttack() == 0);
}

TEST_CASE("Item default type parameter", "[Item]") {
    Item item("Test Item", 5, 10);
    
    REQUIRE(item.GetType() == Item::Type::NONE);
    REQUIRE(item.GetName() == "Test Item");
    REQUIRE(item.GetDefense() == 5);
    REQUIRE(item.GetAttack() == 10);
}
