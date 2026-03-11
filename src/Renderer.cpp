#include "Renderer.hpp"

#include "Map.hpp"

#include <curses.h>

#include <algorithm>
#include <deque>
#include <map>
#include <sstream>
#include <vector>

namespace {

short ToCursesColor(Color color) {
    switch (color) {
    case Color::Black:
        return COLOR_BLACK;
    case Color::DarkGray:
        // There is no true dark gray in the base 8-color palette.
        // Use white without bold to emulate dim gray.
        return COLOR_WHITE;
    case Color::Blue:
    case Color::LightBlue:
        return COLOR_BLUE;
    case Color::Green:
    case Color::LightGreen:
        return COLOR_GREEN;
    case Color::Cyan:
    case Color::LightCyan:
        return COLOR_CYAN;
    case Color::Red:
    case Color::LightRed:
        return COLOR_RED;
    case Color::Magenta:
    case Color::LightMagenta:
        return COLOR_MAGENTA;
    case Color::Brown:
    case Color::Yellow:
        return COLOR_YELLOW;
    case Color::LightGray:
    case Color::White:
        return COLOR_WHITE;
    default:
        return COLOR_WHITE;
    }
}

bool IsBrightColor(Color color) {
    switch (color) {
    case Color::LightGray:
    case Color::LightBlue:
    case Color::LightGreen:
    case Color::LightCyan:
    case Color::LightRed:
    case Color::LightMagenta:
    case Color::Yellow:
    case Color::White:
        return true;
    default:
        return false;
    }
}

std::string FormatSeconds(int totalSeconds) {
    if (totalSeconds < 0) {
        totalSeconds = 0;
    }

    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;
    std::ostringstream os;
    os << minutes << ':';
    if (seconds < 10) {
        os << '0';
    }
    os << seconds;
    return os.str();
}

bool IsWithinViewRadius(const Point& center, int radius, int x, int y) {
    return std::abs(center.x - x) <= radius && std::abs(center.y - y) <= radius;
}

Color ObstacleColor(char symbol) {
    switch (symbol) {
    case '^':
    case 'R':
        return Color::Brown;
    case 'T':
        return Color::Green;
    case '~':
    case 'W':
        return Color::Cyan;
    case '#':
        return Color::LightGray;
    default:
        return Color::LightGray;
    }
}

std::string TruncateWithEllipsis(const std::string& text, size_t maxLen) {
    if (text.size() <= maxLen) {
        return text;
    }

    if (maxLen <= 3) {
        return text.substr(0, maxLen);
    }

    return text.substr(0, maxLen - 3) + "...";
}

std::string MakeProgressBar(
    const std::string& label,
    int current,
    int maximum,
    int barWidth
) {
    if (barWidth < 6) {
        barWidth = 6;
    }

    if (maximum <= 0) {
        maximum = 1;
    }

    current = std::max(0, std::min(current, maximum));

    int filled = (current * barWidth) / maximum;
    if (filled < 0) {
        filled = 0;
    }
    if (filled > barWidth) {
        filled = barWidth;
    }

    std::string bar(static_cast<size_t>(barWidth), '-');
    for (int i = 0; i < filled; ++i) {
        bar[static_cast<size_t>(i)] = '#';
    }

    std::ostringstream os;
    os << label << " [" << bar << "] " << current << '/' << maximum;
    return os.str();
}

} // namespace

Renderer::~Renderer() {
    if (initialized) {
        endwin();
        initialized = false;
    }
}

void Renderer::Init() {
    if (initialized) {
        return;
    }

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    if (has_colors()) {
        start_color();
        use_default_colors();
    }

    initialized = true;
    ClearScreen();
    ApplyColors();
}

void Renderer::SetBackgroundColor(Color color) {
    backgroundColor = color;
    ApplyColors();
}

void Renderer::SetForegroundColor(Color color) {
    foregroundColor = color;
    ApplyColors();
}

void Renderer::ClearScreen() {
    if (!initialized) {
        return;
    }

    erase();
}

void Renderer::PutText(int x, int y, const std::string& text) {
    if (!initialized) {
        return;
    }

    if (y < 0 || x < 0) {
        return;
    }

    mvaddnstr(y, x, text.c_str(), static_cast<int>(text.size()));
}

void Renderer::PutChar(int x, int y, char c){
    if (!initialized) {
        return;
    }

    if (y < 0 || x < 0) {
        return;
    }

    mvaddch(y, x, c);
}

void Renderer::SafePutText(int x, int y, const std::string& text) {
    int width = GetScreenWidth();
    int height = GetScreenHeight();
    if (width <= 0 || height <= 0) {
        return;
    }

    if (x < 0 || y < 0 || x >= width || y >= height) {
        return;
    }

    int available = width - x;
    if (available <= 0) {
        return;
    }

    PutText(x, y, text.substr(0, static_cast<size_t>(available)));
}

void Renderer::PutBox(int x, int y, int width, int height) {
    if (!initialized) {
        return;
    }

    if (width <= 0 || height <= 0) {
        return;
    }

    for (int i = 0; i < height; ++i) {
        mvhline(y + i, x, ' ', width);
    }
}

void Renderer::DrawHud(const Map& map, const std::deque<std::string>& log) {
    const auto& player = map.GetPlayer();
    const int screenWidth = GetScreenWidth();
    const int screenHeight = GetScreenHeight();

    if (screenWidth <= 0 || screenHeight <= 0) {
        return;
    }

    int panelX = 1;
    int panelY = map.GetHeight() + 1;
    if (screenWidth >= map.GetWidth() + 42) {
        panelX = map.GetWidth() + 2;
        panelY = 1;
    }

    const int panelWidth = std::max(16, screenWidth - panelX - 1);
    const int statBarWidth = std::max(8, std::min(20, panelWidth - 14));

    auto fitLine = [&](const std::string& text) {
        return TruncateWithEllipsis(text, static_cast<size_t>(panelWidth));
    };

    std::vector<std::string> statLines;
    statLines.push_back("== HERO STATISTICS ==");
    statLines.push_back(MakeProgressBar("HP", player.GetHealth(), player.GetMaxHealth(), statBarWidth));
    statLines.push_back(MakeProgressBar("XP", player.GetExperience(), player.GetExperienceToNextLevel(), statBarWidth));

    {
        std::ostringstream os;
        os << "LVL " << player.GetLevel() << "  SP " << player.GetUnspentStatPoints()
           << "  VR " << player.ViewRadius();
        statLines.push_back(os.str());
    }

    {
        std::ostringstream os;
        os << "ATK " << player.GetAttack() << "  DEF " << player.GetDefense()
           << "  STR " << player.GetStrength() << "  AGI " << player.GetAgility();
        statLines.push_back(os.str());
    }

    {
        std::ostringstream os;
        os << "Kills " << player.GetMonstersKilled() << "  Dist " << player.GetDistanceTravelled();
        statLines.push_back(os.str());
    }

    {
        std::ostringstream os;
        os << "Items " << player.GetItemsCollected() << "  Time " << FormatSeconds(player.GetPlayTimeSeconds());
        statLines.push_back(os.str());
    }

    {
        std::ostringstream os;
        os << "Inventory " << player.GetInventorySize() << "/" << player.GetMaxInventorySize();
        statLines.push_back(os.str());
    }

    statLines.push_back("Quests:");
    int shownQuests = 0;
    for (const auto& quest : player.GetQuests()) {
        if (quest.IsRewarded()) {
            continue;
        }

        statLines.push_back(" - " + TruncateWithEllipsis(quest.ProgressText(), 40));
        ++shownQuests;
        if (shownQuests >= 2) {
            break;
        }
    }
    if (shownQuests == 0) {
        statLines.push_back(" - none");
    }

    statLines.push_back("Equipment:");
    statLines.push_back(" W: " + player.GetWeapon().GetName());
    statLines.push_back(" A: " + player.GetArmor().GetName());
    statLines.push_back(" H: " + player.GetHelm().GetName());
    statLines.push_back(" B: " + player.GetBoots().GetName());
    statLines.push_back(" S: " + player.GetShield().GetName());

    int y = panelY;
    for (size_t i = 0; i < statLines.size(); ++i) {
        if (y >= screenHeight) {
            return;
        }

        if (i == 0) {
            SetForegroundColor(Color::Yellow);
        } else if (i == 1) {
            SetForegroundColor(Color::LightGreen);
        } else if (i == 2) {
            SetForegroundColor(Color::LightCyan);
        } else if (i == 8) {
            SetForegroundColor(Color::LightMagenta);
        } else if (i == 14) {
            SetForegroundColor(Color::LightCyan);
        } else {
            SetForegroundColor(Color::White);
        }

        SafePutText(panelX, y, fitLine(statLines[i]));
        ++y;
    }

    if (y >= screenHeight) {
        return;
    }

    SetForegroundColor(Color::LightGray);
    SafePutText(panelX, y, fitLine("Messages:"));
    ++y;

    if (y >= screenHeight) {
        return;
    }

    const int messageCapacity = screenHeight - y;
    const size_t start =
        (log.size() > static_cast<size_t>(messageCapacity))
            ? (log.size() - static_cast<size_t>(messageCapacity))
            : 0;

    SetForegroundColor(Color::LightGray);
    for (size_t i = start; i < log.size() && y < screenHeight; ++i) {
        SafePutText(panelX, y, fitLine(std::string("- ") + log[i]));
        ++y;
    }

    SetForegroundColor(Color::White);
}

void Renderer::PrepareScreen(const Map& map, const std::vector<bool>& explored) {
    ClearScreen();
    SetBackgroundColor(Color::Black);
    SetForegroundColor(Color::White);

    Point playerPos = map.GetPlayer().GetPosition();
    int viewRadius = map.GetPlayer().ViewRadius();

    // Draw fog and ground tiles.
    // Free cells always stay black; only map objects/borders behind fog are dimmed.
    for (int y = 0; y < map.GetHeight(); ++y) {
        for (int x = 0; x < map.GetWidth(); ++x) {
            SetForegroundColor(Color::Black);
            SetBackgroundColor(Color::Black);
            PutChar(x, y, ' ');
        }
    }

    // Foreground map symbols are drawn over regular background.
    SetBackgroundColor(Color::Black);

    // Draw map borders only if visible.
    for (int x = 0; x < map.GetWidth(); ++x) {
        bool topVisible = IsWithinViewRadius(playerPos, viewRadius, x, 0);
        bool topExplored = map.IsExplored(explored, x, 0);
        if (topVisible || topExplored) {
            SetForegroundColor(topVisible ? Color::LightGray : Color::DarkGray);
            PutChar(x, 0, '#');
        }

        bool bottomVisible = IsWithinViewRadius(playerPos, viewRadius, x, map.GetHeight() - 1);
        bool bottomExplored = map.IsExplored(explored, x, map.GetHeight() - 1);
        if (bottomVisible || bottomExplored) {
            SetForegroundColor(bottomVisible ? Color::LightGray : Color::DarkGray);
            PutChar(x, map.GetHeight() - 1, '#');
        }
    }

    for (int y = 0; y < map.GetHeight(); ++y) {
        bool leftVisible = IsWithinViewRadius(playerPos, viewRadius, 0, y);
        bool leftExplored = map.IsExplored(explored, 0, y);
        if (leftVisible || leftExplored) {
            SetForegroundColor(leftVisible ? Color::LightGray : Color::DarkGray);
            PutChar(0, y, '#');
        }

        bool rightVisible = IsWithinViewRadius(playerPos, viewRadius, map.GetWidth() - 1, y);
        bool rightExplored = map.IsExplored(explored, map.GetWidth() - 1, y);
        if (rightVisible || rightExplored) {
            SetForegroundColor(rightVisible ? Color::LightGray : Color::DarkGray);
            PutChar(map.GetWidth() - 1, y, '#');
        }
    }

    // Draw obstacles (visible: colored, explored: dimmed).
    for (const auto& obs : map.GetObstacles()) {
        Point pos = obs.GetPosition();
        bool visible = IsWithinViewRadius(playerPos, viewRadius, pos.x, pos.y);
        bool exploredCell = map.IsExplored(explored, pos.x, pos.y);
        if (!visible && !exploredCell) {
            continue;
        }

        SetForegroundColor(visible ? ObstacleColor(obs.GetSymbol()) : Color::DarkGray);
        PutChar(pos.x, pos.y, obs.GetSymbol());
    }

    // Draw chests (visible: yellow, explored: dimmed).
    for (const auto& chest : map.GetChests()) {
        Point pos = chest.GetPosition();
        bool visible = IsWithinViewRadius(playerPos, viewRadius, pos.x, pos.y);
        bool exploredCell = map.IsExplored(explored, pos.x, pos.y);
        if (!visible && !exploredCell) {
            continue;
        }

        SetForegroundColor(visible ? Color::Yellow : Color::DarkGray);
        PutChar(pos.x, pos.y, chest.GetSymbol());
    }

    // Draw NPCs (visible: cyan, explored: dimmed).
    for (const auto& npc : map.GetNpcs()) {
        Point pos = npc.GetPosition();
        bool visible = IsWithinViewRadius(playerPos, viewRadius, pos.x, pos.y);
        bool exploredCell = map.IsExplored(explored, pos.x, pos.y);
        if (!visible && !exploredCell) {
            continue;
        }

        SetForegroundColor(visible ? Color::LightCyan : Color::DarkGray);
        PutChar(pos.x, pos.y, npc.GetSymbol());
    }

    // Draw monsters only when visible.
    SetForegroundColor(Color::LightRed);
    for (const auto& mon : map.GetMonsters()) {
        Point pos = mon.GetPosition();
        if (!IsWithinViewRadius(playerPos, viewRadius, pos.x, pos.y)) {
            continue;
        }

        PutChar(pos.x, pos.y, mon.GetSymbol());
    }

    // Draw player with highlight.
    SetBackgroundColor(Color::Blue);
    SetForegroundColor(Color::Yellow);
    PutChar(playerPos.x, playerPos.y, map.GetPlayer().GetSymbol());

    // Restore default colors for HUD and further drawing.
    SetBackgroundColor(Color::Black);
    SetForegroundColor(Color::White);
}

void Renderer::RenderFrame(const Map& map, const std::vector<bool>& explored, const std::deque<std::string>& messages) {
    PrepareScreen(map, explored);
    DrawHud(map, messages);
    Draw();
}

void Renderer::Draw() {
    if (!initialized) {
        return;
    }

    refresh();
}

void Renderer::ApplyColors() {
    if (!initialized || !has_colors()) {
        return;
    }

    const short fg = ToCursesColor(foregroundColor);
    const short bg = ToCursesColor(backgroundColor);
    const int key = (static_cast<int>(fg) << 8) | static_cast<int>(bg);

    static std::map<int, short> pairByColorKey;
    static short nextPairId = 1;

    short pairId = 1;
    auto it = pairByColorKey.find(key);
    if (it != pairByColorKey.end()) {
        pairId = it->second;
    } else if (nextPairId < COLOR_PAIRS) {
        pairId = nextPairId;
        init_pair(pairId, fg, bg);
        pairByColorKey[key] = pairId;
        ++nextPairId;
    } else if (!pairByColorKey.empty()) {
        // Fallback if terminal has too few color pairs.
        pairId = pairByColorKey.begin()->second;
    } else {
        init_pair(pairId, fg, bg);
        pairByColorKey[key] = pairId;
        nextPairId = 2;
    }

    int attrs = COLOR_PAIR(pairId);
    if (foregroundColor == Color::DarkGray || backgroundColor == Color::DarkGray) {
        attrs |= A_DIM;
    }
    if (IsBrightColor(foregroundColor)) {
        attrs |= A_BOLD;
    }
    attrset(attrs);
}

int Renderer::GetScreenWidth() const {
    if (!initialized) {
        return 0;
    }

    int height = 0;
    int width = 0;
    getmaxyx(stdscr, height, width);
    return width;
}

int Renderer::GetScreenHeight() const {
    if (!initialized) {
        return 0;
    }

    int height = 0;
    int width = 0;
    getmaxyx(stdscr, height, width);
    return height;
}