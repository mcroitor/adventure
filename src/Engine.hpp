#pragma once
#include "Map.hpp"
#include "Player.hpp"

class Engine {
    Map map;
    bool loadedFromSave = false;

public:
    void Init();
    void Run();
};