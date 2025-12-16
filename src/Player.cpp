#include "../include/Player.h"
#include "../include/Tile.h"

Player::Player(float startX, float startY) {
    x = startX;
    y = startY;
    velX = 0;
    velY = 0;
    width = 28;
    height = 28;
    onGround = false;
}

void Player::handleInput(const Uint8* keystate) {
    velX = 0;
    if (keystate[SDL_SCANCODE_LEFT] || keystate[SDL_SCANCODE_A]) {
        velX = -MOVE_SPEED;
    }
    if (keystate[SDL_SCANCODE_RIGHT] || keystate[SDL_SCANCODE_D]) {
        velX = MOVE_SPEED;
    }
    if ((keystate[SDL_SCANCODE_SPACE] || keystate[SDL_SCANCODE_UP] || keystate[SDL_SCANCODE_W]) && onGround) {
        velY = JUMP_FORCE;
        onGround = false;
    }
}

void Player::update(TileMap* map) {
    velY += GRAVITY;   
    x += velX;
    checkCollisionX(map);
    y += velY;
    checkCollisionY(map); 
    if (x < 0) x = 0;
    if (x + width > MAP_WIDTH * TILE_SIZE) x = MAP_WIDTH * TILE_SIZE - width;
}
    
void Player::checkCollisionX(TileMap* map) {
    std::vector<Tile*> tiles = map->getTilesAround(x, y, width, height);    
    for (Tile* tile : tiles) {
        if (!tile->isSolid()) continue;     
        SDL_Rect tileRect = tile->getRect();
        SDL_Rect playerRect = {(int)x, (int)y, width, height};     
        if (SDL_HasIntersection(&playerRect, &tileRect)) {
            if (velX > 0) {
                x = tileRect.x - width;
            } else if (velX < 0) {
                x = tileRect.x + TILE_SIZE;
            }
            velX = 0;
        }
    }
}
    
void Player::checkCollisionY(TileMap* map) {
    std::vector<Tile*> tiles = map->getTilesAround(x, y, width, height);  
    onGround = false;
    for (Tile* tile : tiles) {
        if (!tile->isSolid()) continue;     
        SDL_Rect tileRect = tile->getRect();
        SDL_Rect playerRect = {(int)x, (int)y, width, height};     
        if (SDL_HasIntersection(&playerRect, &tileRect)) {
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
    
bool Player::checkFlagCollision(TileMap* map) {
    std::vector<Tile*> tiles = map->getTilesAround(x, y, width, height);
    for (Tile* tile : tiles) {
        if (tile->type == FLAG) {
            return true;
        }
    }
    return false;
}

void Player::render(SDL_Renderer* renderer, const Camera& camera) {
    SDL_Rect rect = {
        camera.worldToScreenX(x),
        camera.worldToScreenY(y),
        width,
        height
    };
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &rect);
}