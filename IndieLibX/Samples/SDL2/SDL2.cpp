#include <SDL2/SDL.h>
#include <iostream>

// Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main(int argc, char* args[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Create window
    SDL_Window* window = SDL_CreateWindow("SDL2 Mouse and Keyboard Input", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // Main loop flag
    bool quit = false;

    // Event handler
    SDL_Event e;

    // While application is running
    while (!quit) {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
            // User requests quit
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            // User presses a key
            else if (e.type == SDL_KEYDOWN) {
                // Select surfaces based on key press
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        std::cout << "Up arrow key pressed" << std::endl;
                        break;
                    case SDLK_DOWN:
                        std::cout << "Down arrow key pressed" << std::endl;
                        break;
                    case SDLK_LEFT:
                        std::cout << "Left arrow key pressed" << std::endl;
                        break;
                    case SDLK_RIGHT:
                        std::cout << "Right arrow key pressed" << std::endl;
                        break;
                    default:
                        std::cout << "Key pressed: " << SDL_GetKeyName(e.key.keysym.sym) << std::endl;
                        break;
                }
            }
            // User clicks the mouse
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                // Get mouse position
                int x, y;
                SDL_GetMouseState(&x, &y);
                std::cout << "Mouse button pressed at (" << x << ", " << y << ")" << std::endl;
            }
        }
    }

    // Destroy window
    SDL_DestroyWindow(window);

    // Quit SDL subsystems
    SDL_Quit();

    return 0;
}

