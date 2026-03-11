#pragma once

#include <deque>
#include <string>
#include <vector>

class Map;

enum class Color {
    // ASCII colors
    Black = 0,
    Blue = 1,
    Green = 2,
    Cyan = 3,
    Red = 4,
    Magenta = 5,
    Brown = 6,
    LightGray = 7,
    DarkGray = 8,
    LightBlue = 9,
    LightGreen = 10,
    LightCyan = 11,
    LightRed = 12,
    LightMagenta = 13,
    Yellow = 14,
    White = 15
};

class Renderer {
    bool initialized = false;
    Color backgroundColor = Color::Black;
    Color foregroundColor = Color::White;

    void ApplyColors();
    void SafePutText(int x, int y, const std::string& text);
    void PrepareScreen(const Map& map, const std::vector<bool>& explored);
public:
    ~Renderer();

    void Init();
    void SetBackgroundColor(Color color);
    void SetForegroundColor(Color color);
    void ClearScreen();
    void PutText(int x, int y, const std::string& text);
    void PutBox(int x, int y, int width, int height);
    void PutChar(int x, int y, char c);
    void DrawHud(const Map& map, const std::deque<std::string>& log);
    void RenderFrame(const Map& map, const std::vector<bool>& explored, const std::deque<std::string>& messages);
    void Draw();

    int GetScreenWidth() const;
    int GetScreenHeight() const;
};