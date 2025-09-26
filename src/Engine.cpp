#include "Engine.hpp"

#include <iostream>

void Engine::Init() {
    std::cout << "Engine initialized." << std::endl;
    map.Generate();
}

void Engine::Run() {
    std::cout << "Engine running." << std::endl;
}
