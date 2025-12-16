#ifndef ENEMY_H
#define ENEMY_H

#include <SDL2/SDL.h>
#include "Constants.h"
#include "Camera.h"

class TileMap; 

class Enemy {
public:
    float x, y, velX, velY;
    int width, height;
    bool alive;
    int direction;
    bool onGround;

    Enemy(float startX, float startY);
    void update(TileMap* map);
    bool checkPlayerCollision(float px, float py, int pw, int ph);
    void kill();
    bool isPlayerStompingOn(float px, float py, int ph, float pvelY);
    void render(SDL_Renderer* renderer, const Camera& camera);
};

#endif