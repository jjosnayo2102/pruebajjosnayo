#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <iostream>
#include "Constants.h"
#include "Player.h"
#include "Enemy.h"
#include "Tile.h"
#include "Camera.h"

enum GameState {
    PLAYING,
    WIN,
    GAME_OVER
};

class Game {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool isRunning;
    Uint32 frameStart;
    int frameTime;
    
    Player* player;
    TileMap* map;
    Camera camera;
    std::vector<Enemy*> enemies;
    GameState state;

public:
    Game();
    ~Game();

    bool init();
    void run();
    void clean();

private:
    void handleEvents();
    void update();
    void render();
    void restart();
    void renderText(const std::string& text, int x, int y, int size);
};

#endif