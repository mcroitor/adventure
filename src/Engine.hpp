#pragma once
#include "Map.hpp"
#include "Player.hpp"

class Engine {
    Map map;

public:
    void Init();
    void Run();
};