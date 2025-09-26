# Adventure

A simple adventure game written in C++. A Player can move in the Square World, pick up items, fight with monsters, and level up.

## Game representation

- `Engine`: an object that manages the game state, including the player, map, and game loop;
- `Player`: an object that represents the player, with attributes such as position, health, and equipment;
- `Item`: an object that represents an item that can be equipped by the player;
- `Direction`: an enum that represents the directions in which the player can move;
- `Monster`: a class that represents a monster with attributes such as health and attack power;
- `Point`: a class that represents a position on the map;
- `Chest`: a class that represents a treasure chest that can contain items;
- `Obstacle`: a class that represents an obstacle on the map;
- `Renderer`: a class that handles the display of the game on the screen, whether console or GUI;
- `Map`: a class that represents the game world map.

## Compilation and Running

The game can be compiled and run using the following methods:

1. Using `Makefile`:

   ```bash
   make
   ./bin/adventure
   ```

2. Using `CMake`:

   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

    Then run the game with:

    ```bash
   ./bin/adventure
   ```

3. Using `build.cmd`:

   ```cmd
    build.cmd
    .\bin\adventure.exe
   ```
