// ChatGPT-o1-preview: Tekisitkö C++ matopelin käyttäen SDL2 kirjastoa?

#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int CELL_SIZE = 20;

enum Direction { UP, DOWN, LEFT, RIGHT };

struct Point {
    int x, y;
};

class Snake {
public:
    Snake() {
        reset();
    }

    void reset() {
        direction = RIGHT;
        body = { {5, 5}, {4, 5}, {3, 5} };
        grow = false;
    }

    void changeDirection(Direction newDirection) {
        if ((newDirection == UP && direction != DOWN) ||
            (newDirection == DOWN && direction != UP) ||
            (newDirection == LEFT && direction != RIGHT) ||
            (newDirection == RIGHT && direction != LEFT)) {
            direction = newDirection;
        }
    }

    void update() {
        Point newHead = body.front();

        switch (direction) {
        case UP:
            newHead.y -= 1;
            break;
        case DOWN:
            newHead.y += 1;
            break;
        case LEFT:
            newHead.x -= 1;
            break;
        case RIGHT:
            newHead.x += 1;
            break;
        }

        if (newHead.x < 0 || newHead.x >= SCREEN_WIDTH / CELL_SIZE ||
            newHead.y < 0 || newHead.y >= SCREEN_HEIGHT / CELL_SIZE) {
            reset();
            return;
        }

        for (const auto& part : body) {
            if (part.x == newHead.x && part.y == newHead.y) {
                reset();
                return;
            }
        }

        body.insert(body.begin(), newHead);

        if (!grow) {
            body.pop_back();
        } else {
            grow = false;
        }
    }

    void growSnake() {
        grow = true;
    }

    bool checkCollision(const Point& point) const {
        for (const auto& part : body) {
            if (part.x == point.x && part.y == point.y) {
                return true;
            }
        }
        return false;
    }

    void render(SDL_Renderer* renderer) const {
        for (const auto& part : body) {
            SDL_Rect rect = { part.x * CELL_SIZE, part.y * CELL_SIZE, CELL_SIZE, CELL_SIZE };
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_RenderFillRect(renderer, &rect);
        }
    }

private:
    std::vector<Point> body;
    Direction direction;
    bool grow;
};

class Food {
public:
    Food() {
        reset();
    }

    void reset() {
        x = std::rand() % (SCREEN_WIDTH / CELL_SIZE);
        y = std::rand() % (SCREEN_HEIGHT / CELL_SIZE);
    }

    void render(SDL_Renderer* renderer) const {
        SDL_Rect rect = { x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE };
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
    }

    Point getPosition() const {
        return { x, y };
    }

private:
    int x, y;
};

int main(int argc, char* args[]) {
    std::srand(std::time(0));

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool quit = false;
    SDL_Event e;

    Snake snake;
    Food food;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_UP:
                    snake.changeDirection(UP);
                    break;
                case SDLK_DOWN:
                    snake.changeDirection(DOWN);
                    break;
                case SDLK_LEFT:
                    snake.changeDirection(LEFT);
                    break;
                case SDLK_RIGHT:
                    snake.changeDirection(RIGHT);
                    break;
                }
            }
        }

        snake.update();

        if (snake.checkCollision(food.getPosition())) {
            snake.growSnake();
            food.reset();
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        snake.render(renderer);
        food.render(renderer);

        SDL_RenderPresent(renderer);

        SDL_Delay(100);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

