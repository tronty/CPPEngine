//ChatGPT-o1-preview: C++ SDL2 Pacman?

#include <SDL2/SDL.h>
#include <iostream>

// Constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int PACMAN_SIZE = 30;
const int GHOST_SIZE = 30;
const int PACMAN_SPEED = 5;
const int GHOST_SPEED = 3;

// Struct to hold the position and size of Pacman and Ghost
struct Entity {
    int x, y, w, h;
};

void handleInput(bool& running, Entity& pacman) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
        const Uint8* state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_UP]) pacman.y -= PACMAN_SPEED;
        if (state[SDL_SCANCODE_DOWN]) pacman.y += PACMAN_SPEED;
        if (state[SDL_SCANCODE_LEFT]) pacman.x -= PACMAN_SPEED;
        if (state[SDL_SCANCODE_RIGHT]) pacman.x += PACMAN_SPEED;
    }
}

void moveGhost(Entity& ghost, const Entity& pacman) {
    if (ghost.x < pacman.x) ghost.x += GHOST_SPEED;
    if (ghost.x > pacman.x) ghost.x -= GHOST_SPEED;
    if (ghost.y < pacman.y) ghost.y += GHOST_SPEED;
    if (ghost.y > pacman.y) ghost.y -= GHOST_SPEED;
}

bool checkCollision(const Entity& a, const Entity& b) {
    return (a.x < b.x + b.w && a.x + a.w > b.x &&
            a.y < b.y + b.h && a.y + a.h > b.y);
}

int main() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create SDL window and renderer
    SDL_Window* window = SDL_CreateWindow("Pacman Game", SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Initialize game entities
    Entity pacman = {50, 50, PACMAN_SIZE, PACMAN_SIZE};
    Entity ghost = {300, 200, GHOST_SIZE, GHOST_SIZE};

    bool running = true;

    // Main game loop
    while (running) {
        // Handle input
        handleInput(running, pacman);

        // Move ghost
        moveGhost(ghost, pacman);

        // Check collision
        if (checkCollision(pacman, ghost)) {
            std::cout << "Game Over!" << std::endl;
            running = false;
        }

        // Render
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
        SDL_RenderClear(renderer);

        // Draw Pacman
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow
        SDL_Rect pacmanRect = {pacman.x, pacman.y, pacman.w, pacman.h};
        SDL_RenderFillRect(renderer, &pacmanRect);

        // Draw Ghost
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red
        SDL_Rect ghostRect = {ghost.x, ghost.y, ghost.w, ghost.h};
        SDL_RenderFillRect(renderer, &ghostRect);

        SDL_RenderPresent(renderer);

        // Delay for frame rate
        SDL_Delay(16); // ~60 FPS
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

