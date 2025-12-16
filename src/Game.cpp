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
    if (TTF_Init() == -1) {
    std::cout << "Error al inicializar SDL_ttf: " << TTF_GetError() << std::endl;
    }
    font = TTF_OpenFont("../assets/PressStart2P-Regular.ttf", 24); 
    if (!font) {
    std::cout << "Error al cargar la fuente: " << TTF_GetError() << std::endl;
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
    
void Game::renderText(const std::string& text, int x, int y, SDL_Color color) {
    // 1. Validar que la fuente esté cargada
    if (!font) return;
    // 2. Crear una superficie con el texto (Surface es CPU, lento)
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!textSurface) {
        std::cout << "Error creando superficie de texto: " << TTF_GetError() << std::endl;
        return;
    }
    // 3. Convertir superficie a textura (Texture es GPU, rápido)
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture) {
        SDL_FreeSurface(textSurface);
        return;
    }
    // 4. Definir el rectángulo donde se dibujará
    SDL_Rect renderQuad = { x, y, textSurface->w, textSurface->h };
    // 5. Renderizar
    SDL_RenderCopy(renderer, textTexture, NULL, &renderQuad);
    // 6. Limpiar memoria (¡Muy importante para evitar fugas!)
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 92, 148, 252, 255);
    SDL_RenderClear(renderer);
    map->render(renderer, camera);  
    for (Enemy* enemy : enemies) {
        enemy->render(renderer, camera);
    }    
    player->render(renderer, camera);
    if (state == WIN || state == GAME_OVER) {
        // 1. Overlay (Fondo oscuro transparente) - Común para ambos
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200); 
        SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderFillRect(renderer, &overlay);
        // 2. Configurar variables según el estado
        std::string title;
        SDL_Color boxColor;
        int titleX; // Posición X para centrar el título manualmente
        if (state == WIN) {
            title = "GANASTE!";
            boxColor = {0, 255, 0, 255}; // Verde
            titleX = 280;
        } else {
            title = "GAME OVER";
            boxColor = {255, 0, 0, 255}; // Rojo
            titleX = 280;
        }
        // 3. Dibujar la caja de mensaje
        SDL_SetRenderDrawColor(renderer, boxColor.r, boxColor.g, boxColor.b, boxColor.a);
        SDL_Rect msgBox = {200, 200, 400, 200};
        SDL_RenderFillRect(renderer, &msgBox);
        // 4. Renderizar textos usando SDL_ttf
        SDL_Color white = {255, 255, 255, 255};
        // Título principal
        renderText(title, titleX, 250, white);
        // Subtítulo (Instrucción) - Es igual para ambos
        renderText("Presiona [R]", 250, 310, white); // X más al centro
        renderText("para reiniciar", 235, 345, white); // Debajo de la anterior
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
    if (font) TTF_CloseFont(font);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}