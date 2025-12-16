#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include "Constants.h"
#include "Camera.h"

class TileMap; 

class Player {
public:
    float x, y;
    float velX, velY;
    int width, height;
    bool onGround;

    Player(float startX, float startY);
    void handleInput(const Uint8* keystate);
    
    // Aquí pasamos el puntero a TileMap
    void update(TileMap* map); 
    void render(SDL_Renderer* renderer, const Camera& camera);
    bool checkFlagCollision(TileMap* map);

private:
    void checkCollisionX(TileMap* map);
    void checkCollisionY(TileMap* map);
};

#endif