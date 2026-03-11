# Adventure Game Tests

This directory contains unit tests for the Adventure game project using [Catch2](https://github.com/catchorg/Catch2) testing framework.

## Building and Running Tests

### Prerequisites
- CMake 3.10 or higher
- C++20 compatible compiler
- Internet connection (for first build to download Catch2)

### Build Tests

From the project root directory:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Run All Tests

```bash
cd build
ctest
```

Or run the test executable directly:

```bash
./tests/adventure_tests
```

### Run Specific Tests

```bash
# Run only Action tests
./tests/adventure_tests "[Action]"

# List all available tests
./tests/adventure_tests --list-tests

# Run tests with verbose output
./tests/adventure_tests -s
```

## Test Structure

Each test file follows the naming convention `test_<module>.cpp` and tests the corresponding module:

- `test_action.cpp` - Tests for Action enum and conversion functions
- More tests to be added...

## Writing New Tests

1. Create a new file `test_<module>.cpp` in the `tests/` directory
2. Include Catch2 and the module to test:
   ```cpp
   #include <catch2/catch_test_macros.hpp>
   #include "YourModule.hpp"
   ```
3. Write test cases using Catch2 syntax:
   ```cpp
   TEST_CASE("Description of test", "[Tag]") {
       SECTION("Specific aspect") {
           REQUIRE(condition);
       }
   }
   ```
4. Rebuild the project - new tests are automatically discovered

## Catch2 Documentation

- [Tutorial](https://github.com/catchorg/Catch2/blob/devel/docs/tutorial.md)
- [Assertions](https://github.com/catchorg/Catch2/blob/devel/docs/assertions.md)
- [Test Cases and Sections](https://github.com/catchorg/Catch2/blob/devel/docs/test-cases-and-sections.md)

## Current Test Coverage

- ✅ Action - Complete coverage of enum and conversion functions
- ⏳ Item - Planned
- ⏳ Point - Planned
- ⏳ Direction - Planned
- ⏳ Player - Planned
- ⏳ Monster - Planned
- ⏳ Map - Planned
