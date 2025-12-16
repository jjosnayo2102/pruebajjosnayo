#include "../include/Enemy.h"
#include "../include/Tile.h"

Enemy::Enemy(float startX, float startY) {
    x = startX;
    y = startY;
    velY = 0;
    width = 28;
    height = 28;
    alive = true;
    direction = 1; 
    velX = direction * 2.0f; 
    onGround = false;
}
    
void Enemy::update(TileMap* map) {
    if (!alive) return; 
    velY += GRAVITY;
    x += velX;
    std::vector<Tile*> tilesX = map->getTilesAround(x, y, width, height);
    for (Tile* tile : tilesX) {
        if (tile->isSolid()) {
            SDL_Rect tileRect = tile->getRect();
            SDL_Rect enemyRect = {(int)x, (int)y, width, height};
                
            if (SDL_HasIntersection(&enemyRect, &tileRect)) {
                if (velX > 0) { 
                    x = tileRect.x - width;
                } else { 
                    x = tileRect.x + TILE_SIZE;
                }
                direction *= -1;
                velX = direction * 2.0f;
            }
        }
    }

    if (onGround) {
        float lookAheadX = (velX > 0) ? (x + width + 5) : (x - 5);
        float lookAheadY = y + height + 5;
        int col = (int)(lookAheadX / TILE_SIZE);
        int row = (int)(lookAheadY / TILE_SIZE);
        Tile* nextFloor = map->getTile(row, col);
        if (!nextFloor || !nextFloor->isSolid()) {
            direction *= -1;
            velX = direction * 2.0f;
            x += velX; 
        }
    }
    y += velY;
    onGround = false;
    std::vector<Tile*> tilesY = map->getTilesAround(x, y, width, height);
    for (Tile* tile : tilesY) {
        if (tile->isSolid()) {
            SDL_Rect tileRect = tile->getRect();
            SDL_Rect enemyRect = {(int)x, (int)y, width, height};
            if (SDL_HasIntersection(&enemyRect, &tileRect)) {
                if (velY > 0) {
                    y = tileRect.y - height;
                    velY = 0;
                    onGround = true;
                } else if (velY < 0) { 
                    y = tileRect.y + TILE_SIZE;
                    velY = 0;
                }
            }
        }
    }
    if (x <= 0 || x >= MAP_WIDTH * TILE_SIZE - width) {
        direction *= -1;
        velX = direction * 2.0f;
        if (x < 0) x = 0;
    }
}
    

bool Enemy::checkPlayerCollision(float px, float py, int pw, int ph) {
    if (!alive) return false;
    SDL_Rect enemyRect = {(int)x, (int)y, width, height};
    SDL_Rect playerRect = {(int)px, (int)py, pw, ph};
    return SDL_HasIntersection(&enemyRect, &playerRect);
}
    
void Enemy::kill() { alive = false; }
    
bool Enemy::isPlayerStompingOn(float px, float py, int ph, float pvelY) {
    return pvelY > 0 && py + ph < y + height / 2;
}
    
void Enemy::render(SDL_Renderer* renderer, const Camera& camera) {
    if (!alive) return;
    SDL_Rect rect = { camera.worldToScreenX(x), camera.worldToScreenY(y), width, height };
    SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255); // Color marrón Goomba
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect eye1 = {rect.x + (direction == 1 ? 12 : 4), rect.y + 8, 6, 6};
    SDL_Rect eye2 = {rect.x + (direction == 1 ? 20 : 12), rect.y + 8, 6, 6};
    SDL_RenderFillRect(renderer, &eye1);
    SDL_RenderFillRect(renderer, &eye2);
}