#pragma once

class Renderer {
public:
    void Init();
    void SetBackgroundColor(int r, int g, int b);
    void SetForegroundColor(int r, int g, int b);
    void ClearScreen();
    void PutText(int x, int y, const char* text);
    void PutBox(int x, int y, int width, int height);
    void Draw();
};