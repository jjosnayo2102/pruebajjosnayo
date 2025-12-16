#include "../include/Tile.h"

Tile::Tile(int posX, int posY, TileType t) : x(posX), y(posY), type(t) {}
    
SDL_Rect Tile::getRect() const {
    return {x, y, TILE_SIZE, TILE_SIZE};
}
    
bool Tile::isSolid() const {
    return type != EMPTY && type != FLAG;
}
    
void Tile::render(SDL_Renderer* renderer, const Camera& camera) {
    if (type == EMPTY) return;    
    SDL_Rect rect = {
        camera.worldToScreenX(x),
        camera.worldToScreenY(y),
        TILE_SIZE,
        TILE_SIZE
    };   
    if (rect.x + TILE_SIZE < 0 || rect.x > SCREEN_WIDTH) return; 
    switch(type) {
        case BRICK:
            SDL_SetRenderDrawColor(renderer, 200, 76, 12, 255);
            SDL_RenderFillRect(renderer, &rect);
            SDL_SetRenderDrawColor(renderer, 150, 56, 8, 255);
            SDL_RenderDrawRect(renderer, &rect);
            break;
        case GROUND:
            SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);
            SDL_RenderFillRect(renderer, &rect);
            SDL_SetRenderDrawColor(renderer, 101, 50, 15, 255);
            SDL_RenderDrawRect(renderer, &rect);
            break;
        case QUESTION: {
            SDL_SetRenderDrawColor(renderer, 255, 200, 0, 255);
            SDL_RenderFillRect(renderer, &rect);
            SDL_SetRenderDrawColor(renderer, 200, 150, 0, 255);
            SDL_RenderDrawRect(renderer, &rect);
            SDL_Rect question = {rect.x + 12, rect.y + 8, 8, 16};
            SDL_SetRenderDrawColor(renderer, 100, 50, 0, 255);
            SDL_RenderFillRect(renderer, &question);
            break;
        }
        case FLAG: {
            SDL_Rect pole = {rect.x + 14, rect.y, 4, TILE_SIZE};
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
            SDL_RenderFillRect(renderer, &pole);
            SDL_Rect flag = {rect.x + 18, rect.y + 4, 12, 10};
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_RenderFillRect(renderer, &flag);
            break;
        }   
        default:
            break;
    }
}

TileMap::TileMap() {
    tiles.resize(MAP_HEIGHT);
    for (int i = 0; i < MAP_HEIGHT; i++) {
        tiles[i].resize(MAP_WIDTH);
        for (int j = 0; j < MAP_WIDTH; j++) {
            tiles[i][j] = new Tile(j * TILE_SIZE, i * TILE_SIZE, EMPTY);
        }
    }
    createLevel();
}
    
TileMap::~TileMap() {
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            delete tiles[i][j];
        }
    }
}
    
void TileMap::createLevel() {
    for (int j = 0; j < MAP_WIDTH; j++) {
        tiles[17][j]->type = GROUND;
        tiles[18][j]->type = GROUND;
    }

    for (int j = 5; j < 9; j++) tiles[13][j]->type = BRICK;
    for (int j = 12; j < 16; j++) tiles[10][j]->type = BRICK;
    for (int j = 18; j < 22; j++) tiles[7][j]->type = BRICK;
    for (int j = 25; j < 30; j++) tiles[13][j]->type = BRICK;
    for (int j = 32; j < 37; j++) tiles[10][j]->type = BRICK;
    for (int j = 40; j < 44; j++) tiles[8][j]->type = BRICK;

    tiles[10][8]->type = QUESTION;
    tiles[7][14]->type = QUESTION;
    tiles[13][20]->type = QUESTION;
    tiles[10][28]->type = QUESTION;
    tiles[7][35]->type = QUESTION;

    tiles[16][10]->type = BRICK;
    tiles[15][11]->type = BRICK;
    tiles[14][12]->type = BRICK;
    tiles[16][30]->type = BRICK;
    tiles[15][31]->type = BRICK;
    tiles[14][32]->type = BRICK;

    tiles[12][47]->type = FLAG;
    tiles[13][47]->type = FLAG;
    tiles[14][47]->type = FLAG;
    tiles[15][47]->type = FLAG;
    tiles[16][47]->type = FLAG;
}
    
Tile* TileMap::getTile(int row, int col) {
    if (row < 0 || row >= MAP_HEIGHT || col < 0 || col >= MAP_WIDTH) {
        return nullptr;
    }
    return tiles[row][col];
}
    
std::vector<Tile*> TileMap::getTilesAround(float x, float y, int width, int height) {
    std::vector<Tile*> result;     
    int startCol = (int)x / TILE_SIZE;
    int endCol = (int)(x + width) / TILE_SIZE;
    int startRow = (int)y / TILE_SIZE;
    int endRow = (int)(y + height) / TILE_SIZE;    
    for (int row = startRow; row <= endRow; row++) {
        for (int col = startCol; col <= endCol; col++) {
            Tile* tile = getTile(row, col);
            if (tile) {
                result.push_back(tile);
            }
        }
    }  
    return result;
}
    
void TileMap::render(SDL_Renderer* renderer, const Camera& camera) {
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            tiles[i][j]->render(renderer, camera);
        }
    }
}