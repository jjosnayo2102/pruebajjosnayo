#ifndef CONSTANTS_H
#define CONSTANTS_H

// Constantes del juego
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int FPS = 60;
const int FRAME_DELAY = 1000 / FPS;

// Constantes de física
const float GRAVITY = 0.5f;
const float JUMP_FORCE = -12.0f;
const float MOVE_SPEED = 5.0f;

// Constantes de tiles
const int TILE_SIZE = 32;
const int MAP_WIDTH = 50;
const int MAP_HEIGHT = 19;

// Enums compartidos
enum TileType {
    EMPTY = 0,
    BRICK = 1,
    GROUND = 2,
    QUESTION = 3,
    FLAG = 4
};

#endif