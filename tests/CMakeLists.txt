# Test configuration
cmake_minimum_required(VERSION 3.10)

# Download Catch2 if not already present
include(FetchContent)
find_package(Curses REQUIRED)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.0
)
FetchContent_MakeAvailable(Catch2)

# Include parent source directory
include_directories(${CMAKE_SOURCE_DIR}/src)

# Collect all test files
file(GLOB TEST_SOURCES CONFIGURE_DEPENDS "*.cpp")

# Create test executable
add_executable(adventure_tests ${TEST_SOURCES})

# Link with Catch2 and source files (excluding main.cpp)
file(GLOB PROJECT_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")
list(REMOVE_ITEM PROJECT_SOURCES "${CMAKE_SOURCE_DIR}/src/main.cpp")

target_sources(adventure_tests PRIVATE ${PROJECT_SOURCES})
target_include_directories(adventure_tests PRIVATE ${CURSES_INCLUDE_DIRS})
target_link_libraries(adventure_tests PRIVATE Catch2::Catch2WithMain ${CURSES_LIBRARIES})

# Enable testing
enable_testing()
add_test(NAME adventure_tests COMMAND adventure_tests)
