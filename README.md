# Adventure

A simple adventure game written in C++. A Player can move in the Square World, pick up items, fight with monsters, and level up. Initial version is console-based.

## How to Play

After start the game, the Hero profile will be created. Player will distribute initial attribute points and start the adventure.

THe map is represented as a grid of cells. Each cell can contain different elements such as obstacles, monsters, chests, or be empty. The player can move in four directions: up, down, left, and right. In some cases the player will interact with the elements on the map.

The goal is to explore the map, collect items, and defeat monsters. And to develop the character, of course.

Example of the map representation in console:

```text
#######################################
#........R..........TTT..NT....R......#  H - Hero
#........R...........TTT.TT...........#  M - Monster
#....T...R.....C......................#  C - Closed Chest
#....T................R...............#  O - Opened Chest
#....TTTTTT.........WWR...............#  R - Rock (Obstacle)
#....T...............WWWWWW......O....#  T - Tree (Obstacle)
#..MCR................WWC.W......W....#  W - Water (Obstacle)
#..RRR...............WWWWM............#  N - NPC (Non-Player Character)
#........................T............#
#................R....................#
#...........RR.M.RR......T............#
#............R...RR..........R........#
#H...............R...........RNC......#
#######################################
```

### Keys

- `W`: Move Up
- `A`: Move Left
- `S`: Move Down
- `D`: Move Right
- `I`: Open Inventory
- `1` .. `9`: Select option / item in inventory
- `P`: Allocate stat points (then `1`: strength, `2`: agility, `3`: vitality)
- `E`: Interact with object (chest, NPC, etc.)
- `F`: Attack (when near a monster)
- `Q`: Quit Game

## Development

### Game representation

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

### Quest Templates

NPC quests can be configured in `data/quests.txt`.

Line format:

```text
giver|title|description|type|baseTarget|targetLevelDivisor|baseRewardXp|rewardXpPerLevel|useReachPoint
```

- `type`: `KILL_MONSTERS`, `COLLECT_ITEMS`, or `REACH_POSITION`
- `targetLevelDivisor`: if `> 0`, target is increased by `playerLevel / targetLevelDivisor`
- `useReachPoint`: `1/0`, `true/false`, `yes/no`
- `description` for reach quests can use `{x}` and `{y}` placeholders

### Compilation and Running

The game can be compiled and run using the following methods:

Note: the console renderer now uses `ncurses` (`curses.h`), so Curses library must be installed and discoverable by CMake.

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

### Development Tooling

- CI is configured in `.github/workflows/ci.yml`.
- Formatting rules are configured in `.clang-format`.
- Linting rules are configured in `.clang-tidy`.

Useful local commands:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

Run `clang-tidy` during build:

```bash
cmake -S . -B build -DADVENTURE_ENABLE_CLANG_TIDY=ON
cmake --build build
```

Check formatting locally:

```bash
clang-format --dry-run $(git ls-files '*.cpp' '*.hpp' '*.h')
```

### Versioning

- Current project version is stored in `VERSION` and injected into CMake `project()` version.
- You can check version with:

```bash
./bin/adventure --version
```

When preparing a release, bump `VERSION` using semantic versioning (`MAJOR.MINOR.PATCH`).
