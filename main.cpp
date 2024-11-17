#include <emscripten.h>
#include <SDL2/SDL.h>
#include <iostream>
#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
#include <emscripten/html5.h>
#endif

struct Pixel {
    int x, y;
    Uint8 r, g, b;
};

struct GameState {
    bool running;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    std::vector<Pixel> pixels;
    bool mousePressed = false;
    int screenWidth = 800;
    int screenHeight = 600;
};

GameState gameState;

void draw_pixel(SDL_Renderer* renderer, Pixel pixel) {
    SDL_SetRenderDrawColor(renderer, pixel.r, pixel.g, pixel.b, 255);
    SDL_RenderDrawPoint(renderer, pixel.x, pixel.y);
}

extern "C" {
    EMSCRIPTEN_KEEPALIVE
    void resize_renderer(int width, int height) {
        std::cout << "Resizing to: " << width << "x" << height << std::endl;
        gameState.screenWidth = width;
        gameState.screenHeight = height;
        SDL_SetWindowSize(gameState.window, width, height);
        SDL_RenderSetLogicalSize(gameState.renderer, width, height);
    }

    EMSCRIPTEN_KEEPALIVE
    void clear_pixels() {
        gameState.pixels.clear();
    }
}

void game_loop(void* arg) {
    GameState* state = static_cast<GameState*>(arg);
    SDL_Event event;

    SDL_SetRenderDrawColor(state->renderer, 255, 255, 255, 255);
    SDL_RenderClear(state->renderer);

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_MOUSEMOTION:
//                std::cout << "Mouse Motion - Pressed: " << (state->mousePressed ? "true" : "false") 
//                         << " X: " << event.motion.x 
//                         << " Y: " << event.motion.y << std::endl;
                if (state->mousePressed) {
                    Pixel newPixel;
                    newPixel.x = event.motion.x;
                    newPixel.y = event.motion.y;
                    newPixel.r = rand() % 256;
                    newPixel.g = rand() % 256;
                    newPixel.b = rand() % 256;
                    state->pixels.push_back(newPixel);
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    std::cout << "mouse pressed down" << std::endl;
                    state->mousePressed = true;
                    Pixel newPixel;
                    newPixel.x = event.button.x;
                    newPixel.y = event.button.y;
                    newPixel.r = rand() % 256;
                    newPixel.g = rand() % 256;
                    newPixel.b = rand() % 256;
                    state->pixels.push_back(newPixel);
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    std::cout << "mouse released" << std::endl;
                    state->mousePressed = false;
                }
                break;
            case SDL_QUIT:
                state->running = false;
                break;
        }
    } 
    
    for (const Pixel& pixel : state->pixels) {
        draw_pixel(state->renderer, pixel);
    }

    SDL_RenderPresent(state->renderer);
}

#ifdef __EMSCRIPTEN__
void emscripten_loop(void* arg) {
    game_loop(arg);
}
#endif

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    gameState.running = true;
    int width = EM_ASM_INT({ return window.innerWidth });
    int height = EM_ASM_INT({ return window.innerHeight });
    
    if (SDL_CreateWindowAndRenderer(width, height, SDL_WINDOW_SHOWN, &gameState.window, &gameState.renderer) < 0) {
        std::cerr << "failed to create window or renderer" << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_SetWindowTitle(gameState.window, "web game");


#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(emscripten_loop, &gameState, 0, 1);
#else
    while (gameState.running) {
        game_loop(&gameState);
    }
#endif

    SDL_DestroyRenderer(gameState.renderer);
    SDL_DestroyWindow(gameState.window);
    SDL_Quit();

    return 0;
}
