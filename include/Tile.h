#ifndef TILE_H
#define TILE_H

#include <SDL2/SDL.h>
#include <vector>
#include "Constants.h"
#include "Camera.h"

class Tile {
public:
    int x, y;
    TileType type;
    
    Tile(int posX, int posY, TileType t);
    SDL_Rect getRect() const;
    bool isSolid() const;
    void render(SDL_Renderer* renderer, const Camera& camera);
};

class TileMap {
private:
    std::vector<std::vector<Tile*>> tiles;
    
public:
    TileMap();
    ~TileMap();
    
    void createLevel();
    Tile* getTile(int row, int col);
    std::vector<Tile*> getTilesAround(float x, float y, int width, int height);
    void render(SDL_Renderer* renderer, const Camera& camera);
};

#endif