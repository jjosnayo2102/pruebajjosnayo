cmake_minimum_required(VERSION 3.10)
project(mario_game CXX)

include_directories(include)

add_compile_options(-Wall)

set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Player.cpp
    src/Enemy.cpp
    src/Tile.cpp
    src/Camera.cpp
)

add_executable(mario_game ${SOURCES})

target_link_libraries(mario_game mingw32 SDL2main SDL2 SDL2_ttf)