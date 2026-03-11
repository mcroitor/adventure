cmake_minimum_required(VERSION 3.10)

set(ADVENTURE_VERSION "0.1.0")
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/VERSION")
	file(READ "${CMAKE_CURRENT_LIST_DIR}/VERSION" ADVENTURE_VERSION)
	string(STRIP "${ADVENTURE_VERSION}" ADVENTURE_VERSION)
endif()

project(Adventure VERSION ${ADVENTURE_VERSION})
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(ADVENTURE_ENABLE_CLANG_TIDY "Run clang-tidy during CMake builds" OFF)
if(ADVENTURE_ENABLE_CLANG_TIDY)
	find_program(CLANG_TIDY_BIN NAMES clang-tidy)
	if(CLANG_TIDY_BIN)
		set(CMAKE_CXX_CLANG_TIDY
			"${CLANG_TIDY_BIN};--config-file=${CMAKE_CURRENT_LIST_DIR}/.clang-tidy")
		message(STATUS "clang-tidy enabled: ${CLANG_TIDY_BIN}")
	else()
		message(WARNING "ADVENTURE_ENABLE_CLANG_TIDY is ON, but clang-tidy was not found")
	endif()
endif()

find_package(Curses REQUIRED)

# Main executable
file(GLOB SOURCES CONFIGURE_DEPENDS "src/*.cpp")
add_executable(adventure ${SOURCES})
target_compile_definitions(adventure PRIVATE ADVENTURE_VERSION="${PROJECT_VERSION}")
target_include_directories(adventure PRIVATE ${CURSES_INCLUDE_DIRS})
target_link_libraries(adventure PRIVATE ${CURSES_LIBRARIES})

# Enable testing
enable_testing()

# Add tests subdirectory
add_subdirectory(tests)