#include "Engine.hpp"
#include "Listener.hpp"
#include "Renderer.hpp"

#include <algorithm>
#include <chrono>
#include <cctype>
#include <cstdlib>
#include <deque>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace {

void PushMessage(std::deque<std::string>& log, const std::string& message) {
    constexpr size_t kMaxMessages = 24;
    log.push_back(message);
    if (log.size() > kMaxMessages) {
        log.pop_front();
    }
}

enum class StartMenuChoice {
    CONTINUE_SAVE,
    NEW_GAME,
    QUIT
};

struct HeroCustomization {
    std::string name = "Hero";
    int strengthBonus = 0;
    int agilityBonus = 0;
    int vitalityBonus = 0;
};

struct QuestTemplate {
    std::string giver;
    std::string title;
    std::string description;
    Quest::Type type = Quest::Type::KILL_MONSTERS;
    int baseTarget = 1;
    int targetLevelDivisor = 0;
    int baseRewardXp = 0;
    int rewardXpPerLevel = 0;
    bool useReachPoint = false;
};

void DrawCenteredText(Renderer& renderer, int y, const std::string& text) {
    int width = renderer.GetScreenWidth();
    int x = (width > static_cast<int>(text.size()))
        ? (width - static_cast<int>(text.size())) / 2
        : 0;
    renderer.PutText(x, y, text);
}

void ShowHelpScreen(Renderer& renderer, Listener& listener) {
    while (true) {
        renderer.ClearScreen();
        renderer.SetBackgroundColor(Color::Black);
        renderer.SetForegroundColor(Color::Yellow);
        DrawCenteredText(renderer, 2, "HELP");

        renderer.SetForegroundColor(Color::White);
        DrawCenteredText(renderer, 4, "Movement: W A S D");
        DrawCenteredText(renderer, 5, "Attack: F");
        DrawCenteredText(renderer, 6, "Interact: E");
        DrawCenteredText(renderer, 7, "Inventory window: I");
        DrawCenteredText(renderer, 8, "Equipment: K");
        DrawCenteredText(renderer, 9, "Action menu: M");
        DrawCenteredText(renderer, 10, "Use inventory slot: 1..9");
        DrawCenteredText(renderer, 11, "Allocate stats: P then 1/2/3");
        DrawCenteredText(renderer, 12, "Restart run: R (with confirmation)");
        DrawCenteredText(renderer, 13, "Quit and save: Q");
        DrawCenteredText(renderer, 14, "Open this help: F1");

        renderer.SetForegroundColor(Color::LightCyan);
        DrawCenteredText(renderer, 15, "Close help: Esc or F1");
        renderer.Draw();

        const int key = listener.GetKey(-1);
        if (key == 27 || listener.IsToggleHelpInput(key)) {
            return;
        }
    }
}

Action ShowActionMenuScreen(Renderer& renderer, Listener& listener) {
    while (true) {
        renderer.ClearScreen();
        renderer.SetBackgroundColor(Color::Black);
        renderer.SetForegroundColor(Color::Yellow);
        DrawCenteredText(renderer, 2, "ACTION MENU");

        renderer.SetForegroundColor(Color::White);
        DrawCenteredText(renderer, 4, "[1] Attack nearby target");
        DrawCenteredText(renderer, 5, "[2] Interact with nearby object");
        DrawCenteredText(renderer, 6, "[3] Open inventory window");
        DrawCenteredText(renderer, 7, "[4] Open equipment window");
        DrawCenteredText(renderer, 8, "[5] Allocate stat points");
        DrawCenteredText(renderer, 9, "[6] Open help");
        DrawCenteredText(renderer, 10, "[7] Restart run");
        DrawCenteredText(renderer, 11, "[8] Quit and save");

        renderer.SetForegroundColor(Color::LightCyan);
        DrawCenteredText(renderer, 13, "Press 1..8 to choose action");
        DrawCenteredText(renderer, 14, "Close: Esc or M");
        renderer.Draw();

        int key = listener.GetKey(-1);
        if (key == 27 || key == 'm' || key == 'M') {
            return Action::NONE;
        }

        switch (key) {
        case '1': return Action::ATTACK_MONSTER;
        case '2': return Action::INTERACT;
        case '3': return Action::PICK_UP_ITEM;
        case '4': return Action::SHOW_EQUIPMENT;
        case '5': return Action::ALLOCATE_STATS;
        case '6': return Action::TOGGLE_HELP;
        case '7': return Action::RESTART_GAME;
        case '8': return Action::QUIT_GAME;
        default:
            break;
        }
    }
}

bool ShowInventoryScreen(
    Renderer& renderer,
    Listener& listener,
    Player& player,
    std::deque<std::string>& messages
) {
    while (true) {
        renderer.ClearScreen();
        renderer.SetBackgroundColor(Color::Black);
        renderer.SetForegroundColor(Color::Yellow);
        DrawCenteredText(renderer, 2, "INVENTORY");

        renderer.SetForegroundColor(Color::White);
        {
            std::ostringstream os;
            os << "Items " << player.GetInventorySize() << "/" << player.GetMaxInventorySize();
            DrawCenteredText(renderer, 4, os.str());
        }

        const auto& items = player.GetInventory();
        int y = 6;
        if (items.empty()) {
            DrawCenteredText(renderer, y, "Inventory is empty.");
        } else {
            for (size_t i = 0; i < items.size() && i < 9; ++i) {
                std::ostringstream os;
                os << '[' << (i + 1) << "] " << items[i].GetName();
                DrawCenteredText(renderer, y++, os.str());
            }

            if (items.size() > 9) {
                DrawCenteredText(renderer, y, "Only slots 1..9 are quick-usable.");
            }
        }

        renderer.SetForegroundColor(Color::LightGray);
        DrawCenteredText(renderer, 14, "Use item: press 1..9");
        renderer.SetForegroundColor(Color::LightCyan);
        DrawCenteredText(renderer, 15, "Close: Esc or I");
        renderer.Draw();

        int key = listener.GetKey(-1);
        if (key == 27 || key == 'i' || key == 'I') {
            return false;
        }

        if (key >= '1' && key <= '9') {
            int index = key - '1';
            if (index >= player.GetInventorySize()) {
                PushMessage(messages, "No item in this inventory slot.");
                return true;
            }

            std::string itemName = player.GetInventory()[index].GetName();
            player.UseItem(index);
            PushMessage(messages, "Used: " + itemName);
            return true;
        }
    }
}

std::string FormatEquipmentLine(const std::string& slotName, const Item& item) {
    if (item == NO_ITEM) {
        return slotName + ": None";
    }

    std::ostringstream os;
    os << slotName << ": " << item.GetName()
       << " (ATK +" << item.GetAttack() << ", DEF +" << item.GetDefense() << ")";
    return os.str();
}

int EquipmentScore(const Item& item) {
    if (item == NO_ITEM) {
        return 0;
    }

    return item.GetAttack() + item.GetDefense();
}

bool ShowEquipmentScreen(
    Renderer& renderer,
    Listener& listener,
    Player& player,
    std::deque<std::string>& messages
) {
    const std::vector<std::pair<std::string, const Item*>> slots = {
        {"Helm", &player.GetHelm()},
        {"Armor", &player.GetArmor()},
        {"Weapon", &player.GetWeapon()},
        {"Boots", &player.GetBoots()},
        {"Shield", &player.GetShield()}
    };

    int bestSlotIndex = -1;
    int bestScore = 0;
    for (size_t i = 0; i < slots.size(); ++i) {
        int score = EquipmentScore(*slots[i].second);
        if (score > bestScore) {
            bestScore = score;
            bestSlotIndex = static_cast<int>(i);
        }
    }

    while (true) {
        renderer.ClearScreen();
        renderer.SetBackgroundColor(Color::Black);
        renderer.SetForegroundColor(Color::Yellow);
        DrawCenteredText(renderer, 2, "EQUIPMENT");

        renderer.SetForegroundColor(Color::White);
        DrawCenteredText(renderer, 4, "Hero: " + player.GetName());

        int y = 6;
        for (size_t i = 0; i < slots.size(); ++i) {
            renderer.SetForegroundColor(
                (static_cast<int>(i) == bestSlotIndex) ? Color::LightGreen : Color::White
            );
            DrawCenteredText(renderer, y++, FormatEquipmentLine(slots[i].first, *slots[i].second));
        }

        {
            std::ostringstream os;
            os << "HP " << player.GetHealth() << "/" << player.GetMaxHealth()
               << "  STR " << player.GetStrength()
               << "  AGI " << player.GetAgility();
            DrawCenteredText(renderer, 12, os.str());
        }
        {
            std::ostringstream os;
            os << "Total ATK " << player.GetAttack() << "  Total DEF " << player.GetDefense();
            DrawCenteredText(renderer, 13, os.str());
        }

        renderer.SetForegroundColor(Color::LightGray);
        DrawCenteredText(renderer, 14, "Quick use from inventory: press 1..9");

        renderer.SetForegroundColor(Color::LightCyan);
        DrawCenteredText(renderer, 15, "Close: Esc or K");
        renderer.Draw();

        int key = listener.GetKey(-1);
        if (key == 27 || key == 'k' || key == 'K') {
            return false;
        }

        if (key >= '1' && key <= '9') {
            int index = key - '1';
            if (index >= player.GetInventorySize()) {
                PushMessage(messages, "No item in this inventory slot.");
                return true;
            }

            std::string itemName = player.GetInventory()[index].GetName();
            player.UseItem(index);
            PushMessage(messages, "Used: " + itemName);
            return true;
        }
    }
}

void ShowEndOfRunScreen(
    Renderer& renderer,
    Listener& listener,
    const Player& player,
    Color titleColor,
    const std::string& title,
    const std::string& subtitle
) {
    renderer.ClearScreen();
    renderer.SetBackgroundColor(Color::Black);

    renderer.SetForegroundColor(titleColor);
    DrawCenteredText(renderer, 4, title);

    renderer.SetForegroundColor(Color::White);
    DrawCenteredText(renderer, 6, subtitle);

    {
        std::ostringstream os;
        os << "Level reached: " << player.GetLevel();
        DrawCenteredText(renderer, 8, os.str());
    }
    {
        std::ostringstream os;
        os << "Monsters defeated: " << player.GetMonstersKilled();
        DrawCenteredText(renderer, 9, os.str());
    }
    {
        std::ostringstream os;
        os << "Distance travelled: " << player.GetDistanceTravelled();
        DrawCenteredText(renderer, 10, os.str());
    }
    {
        std::ostringstream os;
        os << "Play time: " << player.GetPlayTimeSeconds() << " sec";
        DrawCenteredText(renderer, 11, os.str());
    }

    renderer.SetForegroundColor(Color::LightCyan);
    DrawCenteredText(renderer, 13, "Press any key to exit...");
    renderer.Draw();

    listener.GetKey(-1);
}

void ShowGameOverScreen(Renderer& renderer, Listener& listener, const Player& player) {
    ShowEndOfRunScreen(
        renderer,
        listener,
        player,
        Color::LightRed,
        "GAME OVER",
        player.GetName() + " has fallen."
    );
}

void ShowVictoryScreen(Renderer& renderer, Listener& listener, const Player& player) {
    ShowEndOfRunScreen(
        renderer,
        listener,
        player,
        Color::LightGreen,
        "VICTORY",
        player.GetName() + " cleared the map."
    );
}

StartMenuChoice PromptStartMenu(Renderer& renderer, Listener& listener, bool hasSaveFile) {
    while (true) {
        renderer.ClearScreen();
        renderer.SetBackgroundColor(Color::Black);
        renderer.SetForegroundColor(Color::Yellow);
        DrawCenteredText(renderer, 2, "ADVENTURE");

        renderer.SetForegroundColor(Color::White);
        DrawCenteredText(renderer, 4, "Choose how to start:");

        if (hasSaveFile) {
            DrawCenteredText(renderer, 6, "[1] Continue from save");
            DrawCenteredText(renderer, 7, "[2] New game");
        } else {
            DrawCenteredText(renderer, 6, "[1] New game");
            DrawCenteredText(renderer, 7, "No save file found.");
        }

        DrawCenteredText(renderer, 9, "[Q] Quit");
        renderer.Draw();

        int key = listener.GetKey(-1);
        if (key == -1 || key < 0 || key > 255) {
            continue;
        }

        char c = static_cast<char>(std::toupper(static_cast<unsigned char>(key)));
        if (c == 'Q') {
            return StartMenuChoice::QUIT;
        }

        if (hasSaveFile && c == '1') {
            return StartMenuChoice::CONTINUE_SAVE;
        }
        if (hasSaveFile && c == '2') {
            return StartMenuChoice::NEW_GAME;
        }
        if (!hasSaveFile && c == '1') {
            return StartMenuChoice::NEW_GAME;
        }
    }
}

std::string PromptHeroName(Renderer& renderer, Listener& listener) {
    constexpr size_t kMaxNameLength = 16;
    std::string name;

    while (true) {
        renderer.ClearScreen();
        renderer.SetBackgroundColor(Color::Black);
        renderer.SetForegroundColor(Color::LightCyan);
        DrawCenteredText(renderer, 2, "NEW HERO");

        renderer.SetForegroundColor(Color::White);
        DrawCenteredText(renderer, 4, "Enter hero name and press Enter:");

        std::string inputLine = "Name: " + name + "_";
        DrawCenteredText(renderer, 6, inputLine);
        DrawCenteredText(renderer, 8, "Backspace: delete   Enter: confirm");
        renderer.Draw();

        int key = listener.GetKey(-1);
        if (key == -1) {
            continue;
        }

        if (key == 10 || key == 13) {
            if (name.empty()) {
                return "Hero";
            }
            return name;
        }

        if (key == 8 || key == 127 || key == 263) {
            if (!name.empty()) {
                name.pop_back();
            }
            continue;
        }

        if (key >= 32 && key <= 126 && name.size() < kMaxNameLength) {
            name.push_back(static_cast<char>(key));
        }
    }
}

HeroCustomization PromptHeroCustomization(Renderer& renderer, Listener& listener) {
    HeroCustomization customization;
    customization.name = PromptHeroName(renderer, listener);

    constexpr int kPointsToAllocate = 5;
    int pointsLeft = kPointsToAllocate;

    while (true) {
        int baseStrength = 10;
        int baseAgility = 10;
        int baseHealth = 100;

        renderer.ClearScreen();
        renderer.SetBackgroundColor(Color::Black);
        renderer.SetForegroundColor(Color::LightCyan);
        DrawCenteredText(renderer, 2, "HERO CUSTOMIZATION");

        renderer.SetForegroundColor(Color::White);
        DrawCenteredText(renderer, 4, "Hero: " + customization.name);

        {
            std::ostringstream os;
            os << "Points left: " << pointsLeft;
            DrawCenteredText(renderer, 6, os.str());
        }

        {
            std::ostringstream os;
            os << "[1] Strength: " << (baseStrength + customization.strengthBonus);
            DrawCenteredText(renderer, 8, os.str());
        }
        {
            std::ostringstream os;
            os << "[2] Agility : " << (baseAgility + customization.agilityBonus);
            DrawCenteredText(renderer, 9, os.str());
        }
        {
            std::ostringstream os;
            os << "[3] Vitality: " << (baseHealth + customization.vitalityBonus * 5) << " HP";
            DrawCenteredText(renderer, 10, os.str());
        }

        DrawCenteredText(renderer, 12, "[0] Reset points");
        DrawCenteredText(renderer, 13, "Enter to start game");
        renderer.Draw();

        int key = listener.GetKey(-1);
        if (key == -1 || key < 0 || key > 255) {
            continue;
        }

        char c = static_cast<char>(key);
        if (c == '1' && pointsLeft > 0) {
            ++customization.strengthBonus;
            --pointsLeft;
        } else if (c == '2' && pointsLeft > 0) {
            ++customization.agilityBonus;
            --pointsLeft;
        } else if (c == '3' && pointsLeft > 0) {
            ++customization.vitalityBonus;
            --pointsLeft;
        } else if (c == '0') {
            customization.strengthBonus = 0;
            customization.agilityBonus = 0;
            customization.vitalityBonus = 0;
            pointsLeft = kPointsToAllocate;
        } else if (key == 10 || key == 13) {
            return customization;
        }
    }
}

void ApplyHeroCustomization(Player& player, const HeroCustomization& customization) {
    const int baseStrength = 10;
    const int baseAgility = 10;
    const int baseHealth = 100;

    const int strength = baseStrength + customization.strengthBonus;
    const int agility = baseAgility + customization.agilityBonus;
    const int maxHealth = baseHealth + customization.vitalityBonus * 5;

    player.SetCoreState(customization.name, maxHealth, maxHealth, agility, strength, 5);
    player.SetEquipment(NO_ITEM, NO_ITEM, NO_ITEM, NO_ITEM, NO_ITEM);
}

int RegeneratePlayerHealth(
    Map& map,
    std::chrono::steady_clock::time_point& lastRegenTick
) {
    constexpr auto kRegenInterval = std::chrono::seconds(4);
    constexpr int kHealthPerTick = 1;

    const auto now = std::chrono::steady_clock::now();
    const auto elapsed = now - lastRegenTick;
    if (elapsed < kRegenInterval) {
        return 0;
    }

    const auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
    const auto ticks = elapsedSeconds / kRegenInterval.count();
    if (ticks <= 0) {
        return 0;
    }

    lastRegenTick += kRegenInterval * ticks;

    auto& player = map.GetPlayer();
    if (!player.IsAlive()) {
        return 0;
    }

    int restoredHealth = 0;
    for (int i = 0; i < ticks; ++i) {
        int before = player.GetHealth();
        int after = player.Heal(kHealthPerTick);
        restoredHealth += (after - before);
        if (after >= player.GetMaxHealth()) {
            break;
        }
    }

    return restoredHealth;
}

void TryAutoSave(
    Map& map,
    std::chrono::steady_clock::time_point& lastAutoSaveTick,
    std::deque<std::string>& messages
) {
    constexpr auto kAutoSaveInterval = std::chrono::seconds(45);

    const auto now = std::chrono::steady_clock::now();
    if (now - lastAutoSaveTick < kAutoSaveInterval) {
        return;
    }

    lastAutoSaveTick = now;
    if (map.Save()) {
        PushMessage(messages, "Game auto-saved.");
    } else {
        PushMessage(messages, "Auto-save failed.");
    }
}

bool TryAllocateStatFromAction(Player& player, Action choice) {
    switch (choice) {
    case Action::USE_ITEM_1:
        return player.AllocateStatPoint(Player::StatType::STRENGTH);
    case Action::USE_ITEM_2:
        return player.AllocateStatPoint(Player::StatType::AGILITY);
    case Action::USE_ITEM_3:
        return player.AllocateStatPoint(Player::StatType::VITALITY);
    default:
        return false;
    }
}

int ActionToInventoryIndex(Action action) {
    switch (action) {
    case Action::USE_ITEM_1: return 0;
    case Action::USE_ITEM_2: return 1;
    case Action::USE_ITEM_3: return 2;
    case Action::USE_ITEM_4: return 3;
    case Action::USE_ITEM_5: return 4;
    case Action::USE_ITEM_6: return 5;
    case Action::USE_ITEM_7: return 6;
    case Action::USE_ITEM_8: return 7;
    case Action::USE_ITEM_9: return 8;
    default: return -1;
    }
}

std::string Trim(std::string value) {
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }

    const auto last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

std::string ToUpperAscii(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(std::toupper(c));
    });
    return value;
}

std::vector<std::string> SplitByDelimiter(const std::string& line, char delimiter) {
    std::vector<std::string> parts;
    std::stringstream ss(line);
    std::string part;
    while (std::getline(ss, part, delimiter)) {
        parts.push_back(part);
    }
    return parts;
}

bool TryParseQuestType(const std::string& raw, Quest::Type& outType) {
    const std::string token = ToUpperAscii(Trim(raw));
    if (token == "KILL_MONSTERS") {
        outType = Quest::Type::KILL_MONSTERS;
        return true;
    }
    if (token == "COLLECT_ITEMS") {
        outType = Quest::Type::COLLECT_ITEMS;
        return true;
    }
    if (token == "REACH_POSITION") {
        outType = Quest::Type::REACH_POSITION;
        return true;
    }
    return false;
}

bool ParseBoolToken(const std::string& raw) {
    const std::string token = ToUpperAscii(Trim(raw));
    return token == "1" || token == "TRUE" || token == "YES";
}

void ReplaceAll(std::string& text, const std::string& from, const std::string& to) {
    if (from.empty()) {
        return;
    }

    size_t pos = 0;
    while ((pos = text.find(from, pos)) != std::string::npos) {
        text.replace(pos, from.size(), to);
        pos += to.size();
    }
}

std::vector<QuestTemplate> ParseQuestTemplates(std::istream& input) {
    std::vector<QuestTemplate> templates;
    std::string line;

    while (std::getline(input, line)) {
        line = Trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }

        auto parts = SplitByDelimiter(line, '|');
        if (parts.size() < 9) {
            continue;
        }

        Quest::Type type;
        if (!TryParseQuestType(parts[3], type)) {
            continue;
        }

        QuestTemplate tmpl;
        tmpl.giver = Trim(parts[0]);
        tmpl.title = Trim(parts[1]);
        tmpl.description = Trim(parts[2]);
        tmpl.type = type;
        tmpl.useReachPoint = ParseBoolToken(parts[8]);

        try {
            tmpl.baseTarget = std::max(1, std::stoi(Trim(parts[4])));
            tmpl.targetLevelDivisor = std::max(0, std::stoi(Trim(parts[5])));
            tmpl.baseRewardXp = std::max(0, std::stoi(Trim(parts[6])));
            tmpl.rewardXpPerLevel = std::max(0, std::stoi(Trim(parts[7])));
        } catch (...) {
            continue;
        }

        if (!tmpl.giver.empty() && !tmpl.title.empty()) {
            templates.push_back(tmpl);
        }
    }

    return templates;
}

std::vector<QuestTemplate> LoadQuestTemplates() {
    constexpr const char* kQuestTemplatePaths[] = {
        "data/quests.txt",
        "../data/quests.txt"
    };

    for (const char* path : kQuestTemplatePaths) {
        std::ifstream file(path);
        if (!file.is_open()) {
            continue;
        }

        return ParseQuestTemplates(file);
    }

    return {};
}

const QuestTemplate* FindQuestTemplateForNpc(
    const std::vector<QuestTemplate>& templates,
    const std::string& npcName
) {
    for (const auto& tmpl : templates) {
        if (tmpl.giver == npcName) {
            return &tmpl;
        }
    }

    return nullptr;
}

int NextQuestId(const Player& player) {
    int maxId = 0;
    for (const auto& quest : player.GetQuests()) {
        if (quest.GetId() > maxId) {
            maxId = quest.GetId();
        }
    }
    return maxId + 1;
}

Point FindFarReachPoint(const Map& map) {
    Point playerPos = map.GetPlayer().GetPosition();
    Point best = playerPos;
    int bestDistance = -1;

    for (int y = 1; y < map.GetHeight() - 1; ++y) {
        for (int x = 1; x < map.GetWidth() - 1; ++x) {
            Point candidate{x, y};
            if (!map.IsPassable(candidate)) {
                continue;
            }

            int distance = std::abs(candidate.x - playerPos.x) + std::abs(candidate.y - playerPos.y);
            if (distance > bestDistance) {
                bestDistance = distance;
                best = candidate;
            }
        }
    }

    return best;
}

Quest CreateQuestForNpc(const Map& map, const NPC& npc, const std::vector<QuestTemplate>& templates) {
    const auto& player = map.GetPlayer();
    int questId = NextQuestId(player);

    const QuestTemplate* tmpl = FindQuestTemplateForNpc(templates, npc.GetName());
    if (tmpl != nullptr) {
        int target = tmpl->baseTarget;
        if (tmpl->targetLevelDivisor > 0) {
            target += player.GetLevel() / tmpl->targetLevelDivisor;
        }

        int rewardXp = tmpl->baseRewardXp + player.GetLevel() * tmpl->rewardXpPerLevel;
        bool hasTargetPoint = tmpl->useReachPoint || tmpl->type == Quest::Type::REACH_POSITION;
        Point targetPoint{0, 0};
        std::string description = tmpl->description;

        if (hasTargetPoint) {
            targetPoint = FindFarReachPoint(map);
            ReplaceAll(description, "{x}", std::to_string(targetPoint.x));
            ReplaceAll(description, "{y}", std::to_string(targetPoint.y));
        }

        return Quest(
            questId,
            npc.GetName(),
            tmpl->title,
            description,
            tmpl->type,
            std::max(1, target),
            std::max(0, rewardXp),
            hasTargetPoint,
            targetPoint
        );
    }

    if (npc.GetName() == "Hermit") {
        int target = 2 + (player.GetLevel() / 2);
        int rewardXp = 45 + player.GetLevel() * 6;
        return Quest(
            questId,
            npc.GetName(),
            "Clear Nearby Threats",
            "Defeat monsters around this area.",
            Quest::Type::KILL_MONSTERS,
            target,
            rewardXp
        );
    }

    if (npc.GetName() == "Trader") {
        int target = 2 + (player.GetLevel() / 3);
        int rewardXp = 40 + player.GetLevel() * 5;
        return Quest(
            questId,
            npc.GetName(),
            "Supply Run",
            "Collect useful items for your journey.",
            Quest::Type::COLLECT_ITEMS,
            target,
            rewardXp
        );
    }

    Point targetPoint = FindFarReachPoint(map);
    int rewardXp = 55 + player.GetLevel() * 6;
    std::ostringstream description;
    description << "Reach landmark at (" << targetPoint.x << ", " << targetPoint.y << ").";
    return Quest(
        questId,
        npc.GetName(),
        "Scout Route",
        description.str(),
        Quest::Type::REACH_POSITION,
        1,
        rewardXp,
        true,
        targetPoint
    );
}

void PushQuestCompletionHint(std::deque<std::string>& messages, int newlyCompleted) {
    if (newlyCompleted <= 0) {
        return;
    }

    if (newlyCompleted == 1) {
        PushMessage(messages, "Quest updated: objective complete. Return to quest giver.");
        return;
    }

    std::ostringstream os;
    os << "" << newlyCompleted << " quests completed. Return to quest givers.";
    PushMessage(messages, os.str());
}

void AddQuestProgressLine(std::deque<std::string>& messages, const Quest& quest) {
    std::ostringstream os;
    os << "Quest progress: " << quest.ProgressText();
    PushMessage(messages, os.str());
}

int CountAliveMonsters(const Map& map) {
    int alive = 0;
    for (const auto& monster : map.GetMonsters()) {
        if (monster.IsAlive()) {
            ++alive;
        }
    }
    return alive;
}

int ChebyshevDistance(const Point& a, const Point& b) {
    int dx = std::abs(a.x - b.x);
    int dy = std::abs(a.y - b.y);
    return std::max(dx, dy);
}

bool HasMonsterAtPosition(const Map& map, const Point& position) {
    for (const auto& monster : map.GetMonsters()) {
        if (monster.GetPosition() == position) {
            return true;
        }
    }
    return false;
}

Monster CreateSpawnedMonster(const Point& position, int playerLevel) {
    int levelScale = std::max(0, playerLevel - 1);
    int type = rand() % 3;

    if (type == 0) {
        return Monster(
            position,
            "Goblin",
            50 + levelScale * 4,
            8 + (levelScale / 3),
            12 + (levelScale / 2),
            3,
            25 + levelScale * 2,
            Item("Goblin Dagger", 0, 6 + (levelScale / 2), Item::Type::WEAPON)
        );
    }

    if (type == 1) {
        return Monster(
            position,
            "Orc",
            80 + levelScale * 6,
            6 + (levelScale / 4),
            18 + (levelScale / 2),
            4,
            40 + levelScale * 3,
            Item("Orc Shield", 8 + (levelScale / 2), 0, Item::Type::SHIELD)
        );
    }

    return Monster(
        position,
        "Skeleton",
        40 + levelScale * 3,
        10 + (levelScale / 3),
        10 + (levelScale / 2),
        5,
        30 + levelScale * 2,
        Item("Bone Helm", 4 + (levelScale / 2), 0, Item::Type::HELM)
    );
}

void TrySpawnMonster(
    Map& map,
    std::chrono::steady_clock::time_point& lastSpawnTick,
    int& remainingRespawns,
    std::deque<std::string>& messages
) {
    constexpr auto kSpawnInterval = std::chrono::seconds(20);
    constexpr int kSafeDistanceFromPlayer = 8;
    constexpr int kSpawnAttempts = 128;

    if (remainingRespawns <= 0) {
        return;
    }

    const int maxAliveMonsters = std::max(6, (map.GetWidth() * map.GetHeight()) / 150);
    if (CountAliveMonsters(map) >= maxAliveMonsters) {
        return;
    }

    const auto now = std::chrono::steady_clock::now();
    if (now - lastSpawnTick < kSpawnInterval) {
        return;
    }
    lastSpawnTick = now;

    if (map.GetWidth() <= 2 || map.GetHeight() <= 2) {
        return;
    }

    const Point playerPos = map.GetPlayer().GetPosition();
    for (int attempt = 0; attempt < kSpawnAttempts; ++attempt) {
        Point candidate{
            1 + rand() % (map.GetWidth() - 2),
            1 + rand() % (map.GetHeight() - 2)
        };

        if (candidate == playerPos) {
            continue;
        }

        if (ChebyshevDistance(candidate, playerPos) < kSafeDistanceFromPlayer) {
            continue;
        }

        if (!map.IsPassable(candidate)) {
            continue;
        }

        if (HasMonsterAtPosition(map, candidate)) {
            continue;
        }

        Monster spawned = CreateSpawnedMonster(candidate, map.GetPlayer().GetLevel());
        std::string spawnedName = spawned.GetName();
        map.GetMonsters().push_back(spawned);
        --remainingRespawns;

        std::ostringstream os;
        os << "A " << spawnedName << " spawned nearby. Respawns left: " << remainingRespawns;
        PushMessage(messages, os.str());
        return;
    }
}

}

void Engine::Init() {
    loadedFromSave = false;
}

void Engine::Run() {
    Listener listener;
    Renderer renderer;
    renderer.Init();

    const bool hasSaveFile = std::filesystem::exists("map_save.txt");
    const StartMenuChoice startChoice = PromptStartMenu(renderer, listener, hasSaveFile);
    if (startChoice == StartMenuChoice::QUIT) {
        return;
    }

    bool failedToLoadSave = false;
    if (startChoice == StartMenuChoice::CONTINUE_SAVE) {
        loadedFromSave = map.Load();
        if (!loadedFromSave) {
            failedToLoadSave = true;
        }
    } else {
        loadedFromSave = false;
    }

    if (!loadedFromSave) {
        HeroCustomization customization = PromptHeroCustomization(renderer, listener);
        map.GetPlayer() = Player();
        map.Generate();
        ApplyHeroCustomization(map.GetPlayer(), customization);
    }

    std::vector<bool> explored(
        static_cast<size_t>(map.GetWidth() * map.GetHeight()),
        false
    );
    map.MarkVisibleAsExplored(explored);

    std::deque<std::string> messages;
    PushMessage(messages, "WASD move, F attack, E interact, I inventory, K equipment, M action menu, P allocate stats, R restart, F1 help, Q quit.");
    if (failedToLoadSave) {
        PushMessage(messages, "Save load failed. Started a new game.");
    }
    if (loadedFromSave) {
        PushMessage(messages, "Save loaded.");
    } else {
        PushMessage(messages, "New game started for " + map.GetPlayer().GetName() + ".");
    }

    const std::vector<QuestTemplate> questTemplates = LoadQuestTemplates();
    if (!questTemplates.empty()) {
        std::ostringstream os;
        os << "Loaded quest templates: " << questTemplates.size();
        PushMessage(messages, os.str());
    }

    {
        int activeQuests = 0;
        for (const auto& quest : map.GetPlayer().GetQuests()) {
            if (!quest.IsRewarded()) {
                ++activeQuests;
            }
        }
        if (activeQuests > 0) {
            std::ostringstream os;
            os << "Loaded active quests: " << activeQuests;
            PushMessage(messages, os.str());
        }
    }

    auto gameStart = std::chrono::steady_clock::now();
    auto lastRegenTick = gameStart;
    auto lastAutoSaveTick = gameStart;
    auto lastSpawnTick = gameStart;
    int remainingMonsterRespawns = std::max(12, (map.GetWidth() * map.GetHeight()) / 120);
    bool sessionFinalized = false;

    {
        std::ostringstream os;
        os << "Monster spawning enabled. Respawn pool: " << remainingMonsterRespawns;
        PushMessage(messages, os.str());
    }

    auto finalizeAndAppendStats = [&]() {
        if (sessionFinalized) {
            return;
        }

        sessionFinalized = true;
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - gameStart
        ).count();

        auto& player = map.GetPlayer();
        player.AddPlayTimeSeconds(static_cast<int>(elapsed));

        PushMessage(messages, "=== Session stats ===");

        {
            std::ostringstream os;
            os << "Level: " << player.GetLevel();
            PushMessage(messages, os.str());
        }
        {
            std::ostringstream os;
            os << "Monsters killed: " << player.GetMonstersKilled();
            PushMessage(messages, os.str());
        }
        {
            std::ostringstream os;
            os << "Distance travelled: " << player.GetDistanceTravelled();
            PushMessage(messages, os.str());
        }
        {
            std::ostringstream os;
            os << "Items collected: " << player.GetItemsCollected();
            PushMessage(messages, os.str());
        }
        {
            std::ostringstream os;
            os << "Play time: " << player.GetPlayTimeSeconds() << " sec";
            PushMessage(messages, os.str());
        }
    };

    while(true) {
        map.MarkVisibleAsExplored(explored);
        renderer.RenderFrame(map, explored, messages);
        auto action = listener.GetAction(200);

        if (action == Action::OPEN_ACTION_MENU) {
            action = ShowActionMenuScreen(renderer, listener);
        }

        if (int restoredHealth = RegeneratePlayerHealth(map, lastRegenTick); restoredHealth > 0) {
            std::ostringstream os;
            os << "You regenerate " << restoredHealth << " HP over time.";
            PushMessage(messages, os.str());
        }

        TryAutoSave(map, lastAutoSaveTick, messages);
        TrySpawnMonster(map, lastSpawnTick, remainingMonsterRespawns, messages);

        bool advanceWorld = false;
        bool shouldExit = false;

        switch (action) {
        case Action::MOVE_NORTH:
            map.TryMovePlayer(Direction::NORTH);
            advanceWorld = true;
            break;
        case Action::MOVE_SOUTH:
            map.TryMovePlayer(Direction::SOUTH);
            advanceWorld = true;
            break;
        case Action::MOVE_EAST:
            map.TryMovePlayer(Direction::EAST);
            advanceWorld = true;
            break;
        case Action::MOVE_WEST:
            map.TryMovePlayer(Direction::WEST);
            advanceWorld = true;
            break;
        case Action::ATTACK_MONSTER: {
            auto* target = map.FindAttackTarget(map.GetPlayer().GetPosition());
            if (target == nullptr) {
                PushMessage(messages, "No monster in range.");
                advanceWorld = true;
                break;
            }

            int remainingHealth = map.GetPlayer().Attack(*target);
            {
                std::ostringstream os;
                os << "Attacked " << target->GetName() << ", HP left: " << remainingHealth;
                PushMessage(messages, os.str());
            }

            if (!target->IsAlive()) {
                PushMessage(messages, target->GetName() + " was defeated.");

                auto& player = map.GetPlayer();
                int levelBefore = player.GetLevel();
                int reward = target->GetExperienceReward();
                player.GainExperience(reward);

                {
                    std::ostringstream os;
                    os << "Gained " << reward << " XP. Level: " << player.GetLevel()
                       << " (" << player.GetExperience() << "/" << player.GetExperienceToNextLevel() << ")";
                    PushMessage(messages, os.str());
                }

                if (player.GetLevel() > levelBefore) {
                    std::ostringstream os;
                    os << "Level up! Unspent stat points: " << player.GetUnspentStatPoints();
                    PushMessage(messages, os.str());
                }

                int completedFromKills = player.UpdateQuestProgress(Quest::Type::KILL_MONSTERS, 1);
                PushQuestCompletionHint(messages, completedFromKills);

                Item loot = target->GetDropItem();
                if (!(loot == NO_ITEM)) {
                    auto before = player.GetInventorySize();
                    player.PickUpItem(loot);
                    if (player.GetInventorySize() > before) {
                        PushMessage(messages, "Monster dropped: " + loot.GetName());
                        int completedFromItems = player.UpdateQuestProgress(Quest::Type::COLLECT_ITEMS, 1);
                        PushQuestCompletionHint(messages, completedFromItems);
                    } else {
                        PushMessage(messages, "Monster dropped " + loot.GetName() + ", but inventory is full.");
                    }
                }
            }
            advanceWorld = true;
            break;
        }
        case Action::INTERACT: {
            auto* npc = map.FindInteractableNpc(map.GetPlayer().GetPosition());
            if (npc != nullptr) {
                PushMessage(messages, npc->Talk());

                auto& player = map.GetPlayer();
                Quest* quest = player.FindQuestByGiver(npc->GetName());
                if (quest == nullptr) {
                    Quest newQuest = CreateQuestForNpc(map, *npc, questTemplates);
                    AddQuestProgressLine(messages, newQuest);
                    PushMessage(messages, "New quest: " + newQuest.GetTitle());
                    player.AddQuest(newQuest);
                } else if (quest->IsCompleted() && !quest->IsRewarded()) {
                    int rewardXp = quest->GetRewardExperience();
                    player.GainExperience(rewardXp);
                    quest->SetRewarded(true);

                    std::ostringstream os;
                    os << "Quest complete! Reward: " << rewardXp << " XP.";
                    PushMessage(messages, os.str());
                } else if (!quest->IsCompleted()) {
                    AddQuestProgressLine(messages, *quest);
                } else {
                    PushMessage(messages, "No new quests right now.");
                }

                advanceWorld = true;
                break;
            }

            auto* chest = map.FindInteractableChest(map.GetPlayer().GetPosition());
            if (chest == nullptr) {
                PushMessage(messages, "Nothing to interact with nearby.");
                advanceWorld = true;
                break;
            }

            Item loot = chest->Open();
            if (loot == NO_ITEM) {
                PushMessage(messages, "Chest is empty.");
                break;
            }

            auto before = map.GetPlayer().GetInventorySize();
            map.GetPlayer().PickUpItem(loot);
            if (map.GetPlayer().GetInventorySize() == before) {
                PushMessage(messages, "Inventory is full, cannot pick up: " + loot.GetName());
            } else {
                PushMessage(messages, "Picked up: " + loot.GetName());
                int completedFromItems = map.GetPlayer().UpdateQuestProgress(Quest::Type::COLLECT_ITEMS, 1);
                PushQuestCompletionHint(messages, completedFromItems);
            }
            advanceWorld = true;
            break;
        }
        case Action::PICK_UP_ITEM:
            if (ShowInventoryScreen(renderer, listener, map.GetPlayer(), messages)) {
                advanceWorld = true;
            }
            break;
        case Action::SHOW_EQUIPMENT:
            if (ShowEquipmentScreen(renderer, listener, map.GetPlayer(), messages)) {
                advanceWorld = true;
            }
            break;
        case Action::ALLOCATE_STATS: {
            auto& player = map.GetPlayer();
            if (player.GetUnspentStatPoints() <= 0) {
                PushMessage(messages, "No stat points available.");
                break;
            }

            PushMessage(messages, "Allocate point: 1-Strength, 2-Agility, 3-Vitality");
            renderer.RenderFrame(map, explored, messages);
            Action allocationChoice = listener.GetAction();
            if (TryAllocateStatFromAction(player, allocationChoice)) {
                std::ostringstream os;
                os << "Stat allocated. Remaining points: " << player.GetUnspentStatPoints();
                PushMessage(messages, os.str());
                advanceWorld = true;
            } else {
                PushMessage(messages, "Invalid stat allocation choice.");
            }
            break;
        }
        case Action::RESTART_GAME: {
            PushMessage(messages, "Restart current run? Y/N");
            renderer.RenderFrame(map, explored, messages);

            int key = listener.GetKey(-1);
            char c = (key >= 0 && key <= 255)
                ? static_cast<char>(std::toupper(static_cast<unsigned char>(key)))
                : '\0';

            if (c != 'Y') {
                PushMessage(messages, "Restart canceled.");
                break;
            }

            HeroCustomization customization = PromptHeroCustomization(renderer, listener);
            map.GetPlayer() = Player();
            map.Generate();
            ApplyHeroCustomization(map.GetPlayer(), customization);

            explored.assign(static_cast<size_t>(map.GetWidth() * map.GetHeight()), false);
            map.MarkVisibleAsExplored(explored);

            messages.clear();
            PushMessage(messages, "WASD move, F attack, E interact, I inventory, K equipment, M action menu, P allocate stats, R restart, F1 help, Q quit.");
            PushMessage(messages, "New game started for " + map.GetPlayer().GetName() + ".");

            gameStart = std::chrono::steady_clock::now();
            lastRegenTick = gameStart;
            lastAutoSaveTick = gameStart;
            lastSpawnTick = gameStart;
            remainingMonsterRespawns = std::max(12, (map.GetWidth() * map.GetHeight()) / 120);
            {
                std::ostringstream os;
                os << "Monster spawning enabled. Respawn pool: " << remainingMonsterRespawns;
                PushMessage(messages, os.str());
            }
            sessionFinalized = false;
            break;
        }
        case Action::TOGGLE_HELP:
            ShowHelpScreen(renderer, listener);
            break;
        default:
            if (int index = ActionToInventoryIndex(action); index >= 0) {
                auto& player = map.GetPlayer();
                if (index >= player.GetInventorySize()) {
                    PushMessage(messages, "No item in this inventory slot.");
                    advanceWorld = true;
                    break;
                }

                std::string itemName = player.GetInventory()[index].GetName();
                player.UseItem(index);
                PushMessage(messages, "Used: " + itemName);
                advanceWorld = true;
                break;
            }

            break;
        }

        if (action == Action::QUIT_GAME) {
            finalizeAndAppendStats();
            if (map.Save()) {
                PushMessage(messages, "Game saved.");
            } else {
                PushMessage(messages, "Failed to save game.");
            }
            PushMessage(messages, "Quitting game.");
            shouldExit = true;
        }

        if (!shouldExit && advanceWorld) {
            int reachedObjectives = map.GetPlayer().UpdateReachQuestProgress(map.GetPlayer().GetPosition());
            PushQuestCompletionHint(messages, reachedObjectives);
            map.ProcessMonstersTurn(messages);
        }

        if (!shouldExit && !map.GetPlayer().IsAlive()) {
            PushMessage(messages, "Game Over. The hero has fallen.");
            finalizeAndAppendStats();
            ShowGameOverScreen(renderer, listener, map.GetPlayer());
            break;
        }

        if (!shouldExit && map.AreAllMonstersDefeated() && remainingMonsterRespawns <= 0) {
            PushMessage(messages, "Victory! All monsters are defeated.");
            finalizeAndAppendStats();
            ShowVictoryScreen(renderer, listener, map.GetPlayer());
            break;
        }

        if (shouldExit) {
            map.MarkVisibleAsExplored(explored);
            renderer.RenderFrame(map, explored, messages);
            break;
        }
    }
}
