#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <string>

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

// Estados del juego
enum GameState {
    PLAYING,
    WIN,
    GAME_OVER
};

// Tipos de tiles
enum TileType {
    EMPTY = 0,
    BRICK = 1,
    GROUND = 2,
    QUESTION = 3,
    FLAG = 4
};

// Cámara
class Camera {
public:
    float x, y;
    int width, height;
    
    Camera() : x(0), y(0), width(SCREEN_WIDTH), height(SCREEN_HEIGHT) {}
    
    void follow(float targetX, float targetY) {
        x = targetX - width / 2;
        if (x < 0) x = 0;
        int maxX = (MAP_WIDTH * TILE_SIZE) - width;
        if (x > maxX) x = maxX;
        y = 0;
    }
    
    int worldToScreenX(float worldX) const {
        return (int)(worldX - x);
    }
    
    int worldToScreenY(float worldY) const {
        return (int)(worldY - y);
    }
};

class Tile {
public:
    int x, y;
    TileType type;
    
    Tile(int posX, int posY, TileType t) : x(posX), y(posY), type(t) {}
    
    SDL_Rect getRect() const {
        return {x, y, TILE_SIZE, TILE_SIZE};
    }
    
    bool isSolid() const {
        return type != EMPTY && type != FLAG;
    }
    
    void render(SDL_Renderer* renderer, const Camera& camera) {
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
                // Poste de la bandera
                SDL_Rect pole = {rect.x + 14, rect.y, 4, TILE_SIZE};
                SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
                SDL_RenderFillRect(renderer, &pole);
                // Bandera
                SDL_Rect flag = {rect.x + 18, rect.y + 4, 12, 10};
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                SDL_RenderFillRect(renderer, &flag);
                break;
            }
        }
    }
};

class TileMap {
private:
    std::vector<std::vector<Tile*>> tiles;
    
public:
    TileMap() {
        tiles.resize(MAP_HEIGHT);
        for (int i = 0; i < MAP_HEIGHT; i++) {
            tiles[i].resize(MAP_WIDTH);
            for (int j = 0; j < MAP_WIDTH; j++) {
                tiles[i][j] = new Tile(j * TILE_SIZE, i * TILE_SIZE, EMPTY);
            }
        }
        createLevel();
    }
    
    ~TileMap() {
        for (int i = 0; i < MAP_HEIGHT; i++) {
            for (int j = 0; j < MAP_WIDTH; j++) {
                delete tiles[i][j];
            }
        }
    }
    
    void createLevel() {
        // Suelo completo
        for (int j = 0; j < MAP_WIDTH; j++) {
            tiles[17][j]->type = GROUND;
            tiles[18][j]->type = GROUND;
        }
        
        // Plataformas
        for (int j = 5; j < 9; j++) tiles[13][j]->type = BRICK;
        for (int j = 12; j < 16; j++) tiles[10][j]->type = BRICK;
        for (int j = 18; j < 22; j++) tiles[7][j]->type = BRICK;
        for (int j = 25; j < 30; j++) tiles[13][j]->type = BRICK;
        for (int j = 32; j < 37; j++) tiles[10][j]->type = BRICK;
        for (int j = 40; j < 44; j++) tiles[8][j]->type = BRICK;
        
        // Bloques de pregunta
        tiles[10][8]->type = QUESTION;
        tiles[7][14]->type = QUESTION;
        tiles[13][20]->type = QUESTION;
        tiles[10][28]->type = QUESTION;
        tiles[7][35]->type = QUESTION;
        
        // Escaleras
        tiles[16][10]->type = BRICK;
        tiles[15][11]->type = BRICK;
        tiles[14][12]->type = BRICK;
        tiles[16][30]->type = BRICK;
        tiles[15][31]->type = BRICK;
        tiles[14][32]->type = BRICK;
        
        // BANDERA al final del nivel (columna 47-48)
        tiles[12][47]->type = FLAG;
        tiles[13][47]->type = FLAG;
        tiles[14][47]->type = FLAG;
        tiles[15][47]->type = FLAG;
        tiles[16][47]->type = FLAG;
    }
    
    Tile* getTile(int row, int col) {
        if (row < 0 || row >= MAP_HEIGHT || col < 0 || col >= MAP_WIDTH) {
            return nullptr;
        }
        return tiles[row][col];
    }
    
    std::vector<Tile*> getTilesAround(float x, float y, int width, int height) {
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
    
    void render(SDL_Renderer* renderer, const Camera& camera) {
        for (int i = 0; i < MAP_HEIGHT; i++) {
            for (int j = 0; j < MAP_WIDTH; j++) {
                tiles[i][j]->render(renderer, camera);
            }
        }
    }
};

class Enemy {
public:
    float x, y;
    float velX, velY; // Añadido velY para física real
    int width, height;
    bool alive;
    int direction;
    bool onGround; // Para saber si estamos pisando algo

    Enemy(float startX, float startY) {
        x = startX;
        y = startY;
        velY = 0;
        width = 28;
        height = 28;
        alive = true;
        direction = 1; // 1 derecha, -1 izquierda
        velX = direction * 2.0f; // Velocidad un poco más rápida
        onGround = false;
    }
    
    void update(TileMap* map) {
        if (!alive) return;
        
        // --- 1. APLICAR GRAVEDAD ---
        velY += GRAVITY;
        
        // --- 2. MOVIMIENTO HORIZONTAL (Eje X) ---
        x += velX;
        
        // Colisión con paredes (Eje X)
        std::vector<Tile*> tilesX = map->getTilesAround(x, y, width, height);
        for (Tile* tile : tilesX) {
            if (tile->isSolid()) {
                SDL_Rect tileRect = tile->getRect();
                SDL_Rect enemyRect = {(int)x, (int)y, width, height};
                
                if (SDL_HasIntersection(&enemyRect, &tileRect)) {
                    // Si choca horizontalmente, corregir posición e invertir dirección
                    if (velX > 0) { // Iba a la derecha
                        x = tileRect.x - width;
                    } else { // Iba a la izquierda
                        x = tileRect.x + TILE_SIZE;
                    }
                    direction *= -1;
                    velX = direction * 2.0f;
                }
            }
        }

        // --- 3. DETECCIÓN DE PRECIPICIOS (Lógica "Tortuga Roja") ---
        // Solo comprobamos bordes si estamos apoyados en el suelo
        if (onGround) {
            // Miramos un punto justo delante de los pies del enemigo
            float lookAheadX = (velX > 0) ? (x + width + 5) : (x - 5);
            float lookAheadY = y + height + 5; // Un poco más abajo de los pies

            int col = (int)(lookAheadX / TILE_SIZE);
            int row = (int)(lookAheadY / TILE_SIZE);

            Tile* nextFloor = map->getTile(row, col);

            // Si el tile de abajo-adelante NO es sólido (es aire o no existe), damos la vuelta
            if (!nextFloor || !nextFloor->isSolid()) {
                direction *= -1;
                velX = direction * 2.0f;
                // Pequeño empujón para asegurar que no se quede atascado en el borde
                x += velX; 
            }
        }

        // --- 4. MOVIMIENTO VERTICAL (Eje Y) ---
        y += velY;
        onGround = false; // Asumimos que estamos en el aire hasta chocar con suelo

        // Colisión con suelo/techo (Eje Y)
        std::vector<Tile*> tilesY = map->getTilesAround(x, y, width, height);
        for (Tile* tile : tilesY) {
            if (tile->isSolid()) {
                SDL_Rect tileRect = tile->getRect();
                SDL_Rect enemyRect = {(int)x, (int)y, width, height};
                
                if (SDL_HasIntersection(&enemyRect, &tileRect)) {
                    if (velY > 0) { // Estaba cayendo
                        y = tileRect.y - height;
                        velY = 0;
                        onGround = true;
                    } else if (velY < 0) { // Estaba subiendo/saltando
                        y = tileRect.y + TILE_SIZE;
                        velY = 0;
                    }
                }
            }
        }
        
        // Límite de mapa horizontal
        if (x <= 0 || x >= MAP_WIDTH * TILE_SIZE - width) {
            direction *= -1;
            velX = direction * 2.0f;
            if (x < 0) x = 0;
        }
    }
    
    // ... (Mantén el resto de métodos checkPlayerCollision, kill, render igual que antes)
    bool checkPlayerCollision(float px, float py, int pw, int ph) {
        if (!alive) return false;
        SDL_Rect enemyRect = {(int)x, (int)y, width, height};
        SDL_Rect playerRect = {(int)px, (int)py, pw, ph};
        return SDL_HasIntersection(&enemyRect, &playerRect);
    }
    
    void kill() { alive = false; }
    
    bool isPlayerStompingOn(float px, float py, int ph, float pvelY) {
        return pvelY > 0 && py + ph < y + height / 2;
    }
    
    void render(SDL_Renderer* renderer, const Camera& camera) {
        if (!alive) return;
        SDL_Rect rect = { camera.worldToScreenX(x), camera.worldToScreenY(y), width, height };
        SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255); // Color marrón Goomba
        SDL_RenderFillRect(renderer, &rect);
        // Ojos
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_Rect eye1 = {rect.x + (direction == 1 ? 12 : 4), rect.y + 8, 6, 6};
        SDL_Rect eye2 = {rect.x + (direction == 1 ? 20 : 12), rect.y + 8, 6, 6};
        SDL_RenderFillRect(renderer, &eye1);
        SDL_RenderFillRect(renderer, &eye2);
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
        velY += GRAVITY;
        
        x += velX;
        checkCollisionX(map);
        
        y += velY;
        checkCollisionY(map);
        
        if (x < 0) x = 0;
        if (x + width > MAP_WIDTH * TILE_SIZE) x = MAP_WIDTH * TILE_SIZE - width;
    }
    
    void checkCollisionX(TileMap* map) {
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
    
    void checkCollisionY(TileMap* map) {
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
    
    bool checkFlagCollision(TileMap* map) {
        std::vector<Tile*> tiles = map->getTilesAround(x, y, width, height);
        
        for (Tile* tile : tiles) {
            if (tile->type == FLAG) {
                return true;
            }
        }
        return false;
    }

    void render(SDL_Renderer* renderer, const Camera& camera) {
        SDL_Rect rect = {
            camera.worldToScreenX(x),
            camera.worldToScreenY(y),
            width,
            height
        };
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
    Camera camera;
    std::vector<Enemy*> enemies;
    GameState state;

public:
    Game() : window(nullptr), renderer(nullptr), isRunning(false), 
             player(nullptr), map(nullptr), state(PLAYING) {}
    
    ~Game() {
        clean();
    }

    bool init() {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "Error al inicializar SDL: " << SDL_GetError() << std::endl;
            return false;
        }

        window = SDL_CreateWindow(
            "Super Mario Bros Clone",
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

        map = new TileMap();
        player = new Player(50, 100);
        
        enemies.clear(); // Asegúrate de limpiar el vector si es restart

        // Altura del suelo base: Fila 17 * 32 = 544. Enemigo altura 28 -> y = 516
        // Plataforma 1: Fila 13, Cols 5-9. y = 13*32 - 28 = 388. x ~ 6*32 = 192
        enemies.push_back(new Enemy(192, 388)); 

        // Plataforma 2: Fila 10, Cols 12-16. y = 10*32 - 28 = 292. x ~ 13*32 = 416
        enemies.push_back(new Enemy(416, 292));

        // Plataforma 3: Fila 7, Cols 18-22. y = 7*32 - 28 = 196. x ~ 20*32 = 640
        enemies.push_back(new Enemy(640, 196));

        // Suelo normal más adelante
        enemies.push_back(new Enemy(850, 516));
        enemies.push_back(new Enemy(1200, 516));

        isRunning = true;
        std::cout << "¡Juego iniciado! Llega a la bandera para ganar" << std::endl;
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
                    // Reiniciar con R
                    if (event.key.keysym.sym == SDLK_r && state != PLAYING) {
                        restart();
                    }
                    break;
            }
        }

        if (state == PLAYING) {
            const Uint8* keystate = SDL_GetKeyboardState(NULL);
            player->handleInput(keystate);
        }
    }

    void update() {
        if (state != PLAYING) return;
        
        player->update(map);
        
        // Verificar victoria (tocar la bandera)
        if (player->checkFlagCollision(map)) {
            state = WIN;
            std::cout << "¡GANASTE!" << std::endl;
            return;
        }
        
        // Verificar caída al vacío
        if (player->y > SCREEN_HEIGHT + 100) {
            state = GAME_OVER;
            std::cout << "¡Caíste al vacío!" << std::endl;
            return;
        }
        
        // Actualizar enemigos
        for (Enemy* enemy : enemies) {
            enemy->update(map);
            
            if (enemy->checkPlayerCollision(player->x, player->y, player->width, player->height)) {
                if (enemy->isPlayerStompingOn(player->x, player->y, player->height, player->velY)) {
                    enemy->kill();
                    player->velY = -8;
                    std::cout << "¡Enemigo derrotado!" << std::endl;
                } else if (enemy->alive) {
                    state = GAME_OVER;
                    std::cout << "¡Te golpeó un enemigo!" << std::endl;
                    return;
                }
            }
        }
        
        camera.follow(player->x + player->width / 2, player->y);
    }
    
    void restart() {
        // Reiniciar jugador
        player->x = 50;
        player->y = 100;
        player->velX = 0;
        player->velY = 0;
        
        // Reiniciar enemigos
        for (Enemy* e : enemies) delete e;
        enemies.clear();

        // Altura del suelo base: Fila 17 * 32 = 544. Enemigo altura 28 -> y = 516
        // Plataforma 1: Fila 13, Cols 5-9. y = 13*32 - 28 = 388. x ~ 6*32 = 192
        enemies.push_back(new Enemy(192, 388)); 

        // Plataforma 2: Fila 10, Cols 12-16. y = 10*32 - 28 = 292. x ~ 13*32 = 416
        enemies.push_back(new Enemy(416, 292));

        // Plataforma 3: Fila 7, Cols 18-22. y = 7*32 - 28 = 196. x ~ 20*32 = 640
        enemies.push_back(new Enemy(640, 196));

        // Suelo normal más adelante
        enemies.push_back(new Enemy(850, 516));
        enemies.push_back(new Enemy(1200, 516));
        
        state = PLAYING;
        camera.x = 0;
        
        std::cout << "¡Juego reiniciado!" << std::endl;
    }
    
    void renderText(const std::string& text, int x, int y, int size) {
        // Texto simple usando rectángulos (simulando letras grandes)
        int startX = x;
        for (char c : text) {
            if (c == ' ') {
                startX += size;
                continue;
            }
            SDL_Rect charRect = {startX, y, size - 2, size};
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &charRect);
            startX += size;
        }
    }

    void render() {
        SDL_SetRenderDrawColor(renderer, 92, 148, 252, 255);
        SDL_RenderClear(renderer);

        map->render(renderer, camera);
        
        for (Enemy* enemy : enemies) {
            enemy->render(renderer, camera);
        }
        
        player->render(renderer, camera);
        
        // Mensajes de estado
        if (state == WIN) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
            SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_RenderFillRect(renderer, &overlay);
            
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_Rect winBox = {200, 200, 400, 200};
            SDL_RenderFillRect(renderer, &winBox);
            
            renderText("GANASTE!", 280, 250, 30);
            renderText("Presiona R para reiniciar", 220, 320, 15);
        } else if (state == GAME_OVER) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
            SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_RenderFillRect(renderer, &overlay);
            
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_Rect loseBox = {200, 200, 400, 200};
            SDL_RenderFillRect(renderer, &loseBox);
            
            renderText("GAME OVER", 250, 250, 30);
            renderText("Presiona R para reiniciar", 220, 320, 15);
        }

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
        for (Enemy* enemy : enemies) delete enemy;
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window) SDL_DestroyWindow(window);
        SDL_Quit();
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