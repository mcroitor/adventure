#include "Engine.hpp"

#include <iostream>
#include <string_view>

int main(int argc, char** argv) {
    if (argc > 1 && std::string_view(argv[1]) == "--version") {
#ifdef ADVENTURE_VERSION
        std::cout << "adventure " << ADVENTURE_VERSION << '\n';
#else
        std::cout << "adventure unknown\n";
#endif
        return 0;
    }

    Engine engine;
    engine.Init();
    engine.Run();
    return 0;
}