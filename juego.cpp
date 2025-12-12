#include <SDL2/SDL.h>
#include <iostream>
#include <vector>

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
const int MAP_WIDTH = 25;   // 800 / 32
const int MAP_HEIGHT = 19;  // 600 / 32 (aprox)

// Tipos de tiles
enum TileType {
    EMPTY = 0,
    BRICK = 1,
    GROUND = 2,
    QUESTION = 3
};

// Clase Tile individual
class Tile {
public:
    int x, y;
    TileType type;
    
    Tile(int posX, int posY, TileType t) : x(posX), y(posY), type(t) {}
    
    SDL_Rect getRect() const {
        return {x, y, TILE_SIZE, TILE_SIZE};
    }
    
    bool isSolid() const {
        return type != EMPTY;
    }
    
    void render(SDL_Renderer* renderer) {
        if (type == EMPTY) return;
        
        SDL_Rect rect = getRect();
        
        switch(type) {
            case BRICK:
                SDL_SetRenderDrawColor(renderer, 200, 76, 12, 255); // Ladrillo naranja
                SDL_RenderFillRect(renderer, &rect);
                // Borde del ladrillo
                SDL_SetRenderDrawColor(renderer, 150, 56, 8, 255);
                SDL_RenderDrawRect(renderer, &rect);
                break;
            case GROUND:
                SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255); // Tierra marrón
                SDL_RenderFillRect(renderer, &rect);
                SDL_SetRenderDrawColor(renderer, 101, 50, 15, 255);
                SDL_RenderDrawRect(renderer, &rect);
                break;
            case QUESTION:
                SDL_SetRenderDrawColor(renderer, 255, 200, 0, 255); // Amarillo
                SDL_RenderFillRect(renderer, &rect);
                SDL_SetRenderDrawColor(renderer, 200, 150, 0, 255);
                SDL_RenderDrawRect(renderer, &rect);
                // Dibujar "?" simplificado
                SDL_Rect question = {rect.x + 12, rect.y + 8, 8, 16};
                SDL_SetRenderDrawColor(renderer, 100, 50, 0, 255);
                SDL_RenderFillRect(renderer, &question);
                break;
        }
    }
};

// Sistema de mapa con tiles
class TileMap {
private:
    std::vector<std::vector<Tile*>> tiles;
    
public:
    TileMap() {
        // Inicializar mapa vacío
        tiles.resize(MAP_HEIGHT);
        for (int i = 0; i < MAP_HEIGHT; i++) {
            tiles[i].resize(MAP_WIDTH);
            for (int j = 0; j < MAP_WIDTH; j++) {
                tiles[i][j] = new Tile(j * TILE_SIZE, i * TILE_SIZE, EMPTY);
            }
        }
        
        // Crear nivel de prueba
        createTestLevel();
    }
    
    ~TileMap() {
        for (int i = 0; i < MAP_HEIGHT; i++) {
            for (int j = 0; j < MAP_WIDTH; j++) {
                delete tiles[i][j];
            }
        }
    }
    
    void createTestLevel() {
        // Suelo (últimas 2 filas)
        for (int j = 0; j < MAP_WIDTH; j++) {
            tiles[17][j]->type = GROUND;
            tiles[18][j]->type = GROUND;
        }
        
        // Plataforma flotante 1
        for (int j = 5; j < 9; j++) {
            tiles[13][j]->type = BRICK;
        }
        
        // Plataforma flotante 2
        for (int j = 12; j < 16; j++) {
            tiles[10][j]->type = BRICK;
        }
        
        // Plataforma flotante 3 (más alta)
        for (int j = 18; j < 22; j++) {
            tiles[7][j]->type = BRICK;
        }
        
        // Bloques de pregunta
        tiles[10][8]->type = QUESTION;
        tiles[7][14]->type = QUESTION;
        tiles[13][20]->type = QUESTION;
        
        // Escalera de ladrillos
        tiles[16][10]->type = BRICK;
        tiles[15][11]->type = BRICK;
        tiles[14][12]->type = BRICK;
    }
    
    Tile* getTile(int row, int col) {
        if (row < 0 || row >= MAP_HEIGHT || col < 0 || col >= MAP_WIDTH) {
            return nullptr;
        }
        return tiles[row][col];
    }
    
    // Obtener tiles alrededor de una posición
    std::vector<Tile*> getTilesAround(float x, float y, int width, int height) {
        std::vector<Tile*> result;
        
        int startCol = (int)x / TILE_SIZE;
        int endCol = (int)(x + width) / TILE_SIZE;
        int startRow = (int)y / TILE_SIZE;
        int endRow = (int)(y + height) / TILE_SIZE;
        
        for (int row = startRow; row <= endRow; row++) {
            for (int col = startCol; col <= endCol; col++) {
                Tile* tile = getTile(row, col);
                if (tile && tile->isSolid()) {
                    result.push_back(tile);
                }
            }
        }
        
        return result;
    }
    
    void render(SDL_Renderer* renderer) {
        for (int i = 0; i < MAP_HEIGHT; i++) {
            for (int j = 0; j < MAP_WIDTH; j++) {
                tiles[i][j]->render(renderer);
            }
        }
    }
};

class Player {
public:
    float x, y;
    float velX, velY;
    int width, height;
    bool onGround;

    Player(float startX, float startY) {
        x = startX;
        y = startY;
        velX = 0;
        velY = 0;
        width = 28;
        height = 28;
        onGround = false;
    }

    void handleInput(const Uint8* keystate) {
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

    void update(TileMap* map) {
        // Aplicar gravedad
        velY += GRAVITY;
        
        // Movimiento horizontal
        x += velX;
        checkCollisionX(map);
        
        // Movimiento vertical
        y += velY;
        checkCollisionY(map);
        
        // Límites de pantalla
        if (x < 0) x = 0;
        if (x + width > SCREEN_WIDTH) x = SCREEN_WIDTH - width;
    }
    
    void checkCollisionX(TileMap* map) {
        std::vector<Tile*> tiles = map->getTilesAround(x, y, width, height);
        
        for (Tile* tile : tiles) {
            SDL_Rect tileRect = tile->getRect();
            SDL_Rect playerRect = {(int)x, (int)y, width, height};
            
            if (SDL_HasIntersection(&playerRect, &tileRect)) {
                if (velX > 0) { // Moviendo a la derecha
                    x = tileRect.x - width;
                } else if (velX < 0) { // Moviendo a la izquierda
                    x = tileRect.x + TILE_SIZE;
                }
                velX = 0;
            }
        }
    }
    
    void checkCollisionY(TileMap* map) {
        std::vector<Tile*> tiles = map->getTilesAround(x, y, width, height);
        
        onGround = false;
        
        for (Tile* tile : tiles) {
            SDL_Rect tileRect = tile->getRect();
            SDL_Rect playerRect = {(int)x, (int)y, width, height};
            
            if (SDL_HasIntersection(&playerRect, &tileRect)) {
                if (velY > 0) { // Cayendo
                    y = tileRect.y - height;
                    velY = 0;
                    onGround = true;
                } else if (velY < 0) { // Subiendo (golpear bloque desde abajo)
                    y = tileRect.y + TILE_SIZE;
                    velY = 0;
                }
            }
        }
    }

    void render(SDL_Renderer* renderer) {
        SDL_Rect rect = {(int)x, (int)y, width, height};
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
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

public:
    Game() : window(nullptr), renderer(nullptr), isRunning(false), player(nullptr), map(nullptr) {}
    
    ~Game() {
        clean();
    }

    bool init() {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "Error al inicializar SDL: " << SDL_GetError() << std::endl;
            return false;
        }

        window = SDL_CreateWindow(
            "Super Mario Bros - Sistema de Tiles",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN
        );

        if (!window) {
            std::cerr << "Error al crear ventana: " << SDL_GetError() << std::endl;
            return false;
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            std::cerr << "Error al crear renderer: " << SDL_GetError() << std::endl;
            return false;
        }

        // Crear mapa y jugador
        map = new TileMap();
        player = new Player(50, 100);

        isRunning = true;
        std::cout << "Sistema de tiles inicializado" << std::endl;
        std::cout << "Controles: Flechas/WASD para mover, Espacio/W para saltar" << std::endl;
        return true;
    }

    void handleEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    isRunning = false;
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        isRunning = false;
                    }
                    break;
            }
        }

        const Uint8* keystate = SDL_GetKeyboardState(NULL);
        player->handleInput(keystate);
    }

    void update() {
        player->update(map);
    }

    void render() {
        // Fondo celeste
        SDL_SetRenderDrawColor(renderer, 92, 148, 252, 255);
        SDL_RenderClear(renderer);

        // Dibujar mapa
        map->render(renderer);
        
        // Dibujar jugador
        player->render(renderer);

        SDL_RenderPresent(renderer);
    }

    void run() {
        while (isRunning) {
            frameStart = SDL_GetTicks();

            handleEvents();
            update();
            render();

            frameTime = SDL_GetTicks() - frameStart;
            if (frameTime < FRAME_DELAY) {
                SDL_Delay(FRAME_DELAY - frameTime);
            }
        }
    }

    void clean() {
        if (player) delete player;
        if (map) delete map;
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window) SDL_DestroyWindow(window);
        SDL_Quit();
        std::cout << "Juego cerrado correctamente" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    Game game;

    if (!game.init()) {
        return -1;
    }

    game.run();

    return 0;
}