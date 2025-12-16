#include "../include/Game.h"

Game::Game() : window(nullptr), renderer(nullptr), isRunning(false), player(nullptr), map(nullptr), state(PLAYING) {}
    
Game::~Game() {
    clean();
}

bool Game::init() {
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
    enemies.clear(); 
    enemies.push_back(new Enemy(192, 388)); 
    enemies.push_back(new Enemy(416, 292));
    enemies.push_back(new Enemy(640, 196));
    enemies.push_back(new Enemy(850, 516));
    enemies.push_back(new Enemy(1200, 516));

    isRunning = true;
    std::cout << "¡Juego iniciado! Llega a la bandera para ganar" << std::endl;
    return true;
}

void Game::handleEvents() {
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

void Game::update() {
    if (state != PLAYING) return;  
    player->update(map);
    if (player->checkFlagCollision(map)) {
        state = WIN;
        std::cout << "¡GANASTE!" << std::endl;
        return;
    }
    if (player->y > SCREEN_HEIGHT + 100) {
        state = GAME_OVER;
        std::cout << "¡Caíste al vacío!" << std::endl;
        return;
    } 
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
    
void Game::restart() {
    player->x = 50;
    player->y = 100;
    player->velX = 0;
    player->velY = 0;
    for (Enemy* e : enemies) delete e;
    enemies.clear();
    enemies.push_back(new Enemy(192, 388)); 
    enemies.push_back(new Enemy(416, 292));
    enemies.push_back(new Enemy(640, 196));
    enemies.push_back(new Enemy(850, 516));
    enemies.push_back(new Enemy(1200, 516));
    state = PLAYING;
    camera.x = 0;  
    std::cout << "¡Juego reiniciado!" << std::endl;
}
    
void Game::renderText(const std::string& text, int x, int y, int size) {
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

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 92, 148, 252, 255);
    SDL_RenderClear(renderer);
    map->render(renderer, camera);  
    for (Enemy* enemy : enemies) {
        enemy->render(renderer, camera);
    }    
    player->render(renderer, camera);
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

void Game::run() {
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

void Game::clean() {
    if (player) delete player;
    if (map) delete map;
    for (Enemy* enemy : enemies) delete enemy;
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}