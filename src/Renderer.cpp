#include "Renderer.hpp"

#include <string>
#include <iostream>

std::string Renderer::ToASCIIBackgroundColor(Color color) {
    return "\033[4" + std::to_string(static_cast<int>(color)) + "m";
}

std::string Renderer::ToASCIIForegroundColor(Color color) {
    return "\033[3" + std::to_string(static_cast<int>(color)) + "m";
}

std::string Renderer::ToCursorPosition(int x, int y) {
    return "\033[" + std::to_string(y + 1) + ";" + std::to_string(x + 1) + "H";
}

void Renderer::Init() {
    // Initialize the renderer
    buffer.clear();
    buffer += "\033[2J"; // Clear screen
    buffer += ToASCIIBackgroundColor(backgroundColor);
    buffer += ToASCIIForegroundColor(foregroundColor);
}

void Renderer::SetBackgroundColor(Color color) {
    backgroundColor = color;
}

void Renderer::SetForegroundColor(Color color) {
    foregroundColor = color;
}

void Renderer::ClearScreen() {
    std::cout << "\033[2J";
}

void Renderer::PutText(int x, int y, const char* text) {
    buffer += ToCursorPosition(x, y);
    buffer += text;
}

void Renderer::PutBox(int x, int y, int width, int height) {
    buffer += ToASCIIBackgroundColor(backgroundColor);
    buffer += ToASCIIForegroundColor(foregroundColor);
    for (int i = 0; i < height; ++i) {
        buffer += ToCursorPosition(x, y + i);
        buffer += std::string(width, ' ');
    }
}

void Renderer::Draw() {
    // Present the rendered content to the screen
    std::string output;
    output += buffer;
    output += "\033[0m"; // Reset colors
    // Output to console (for example)
    std::cout << output;
}