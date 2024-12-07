#include <SDL2/SDL.h>
#include <iostream>

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Create an SDL window
    SDL_Window* window = SDL_CreateWindow("SDL2 Input Example",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          800, 600,
                                          SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Failed to create SDL window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // Create an SDL renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Failed to create SDL renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    bool running = true;
    SDL_Event event;

    while (running) {
        // Poll events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Get mouse state
        int mouseX, mouseY;
        Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);

        // Check mouse buttons
        bool leftButton = mouseState & SDL_BUTTON(SDL_BUTTON_LEFT);
        bool rightButton = mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT);

        // Print mouse state
        std::cout << "Mouse Position: (" << mouseX << ", " << mouseY << ")\n";
        std::cout << "Left Button: " << (leftButton ? "Pressed" : "Released") << "\n";
        std::cout << "Right Button: " << (rightButton ? "Pressed" : "Released") << "\n";

        // Get keyboard state
        const Uint8* keyboardState = SDL_GetKeyboardState(nullptr);

        // Check specific keys
        bool isWPressed = keyboardState[SDL_SCANCODE_W];
        bool isSPressed = keyboardState[SDL_SCANCODE_S];

        // Print keyboard state
        std::cout << "W Key: " << (isWPressed ? "Pressed" : "Released") << "\n";
        std::cout << "S Key: " << (isSPressed ? "Pressed" : "Released") << "\n";

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Present the screen
        SDL_RenderPresent(renderer);

        // Add a short delay
        SDL_Delay(100);
    }

    // Clean up and quit SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

