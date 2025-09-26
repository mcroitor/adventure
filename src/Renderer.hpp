#pragma once

#include <string>

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
    std::string buffer;
    Color backgroundColor = Color::Black;
    Color foregroundColor = Color::White;

    std::string ToASCIIBackgroundColor(Color color);
    std::string ToASCIIForegroundColor(Color color);
    std::string ToCursorPosition(int x, int y);
public:
    void Init();
    void SetBackgroundColor(Color color);
    void SetForegroundColor(Color color);
    void ClearScreen();
    void PutText(int x, int y, const char* text);
    void PutBox(int x, int y, int width, int height);
    void Draw();
};