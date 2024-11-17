#include <emscripten.h>
#include <SDL2/SDL.h>
#include <iostream>
#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
#include <emscripten/html5.h>
#endif

struct GameState {
    bool running;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* drawingTexture = nullptr;
    bool mousePressed = false;
    int screenWidth = 800;
    int screenHeight = 600;
    int lastMouseX = -1;
    int lastMouseY = -1;
};

GameState gameState;

extern "C" {
    EMSCRIPTEN_KEEPALIVE
    void resize_renderer(int width, int height) {
//        std::cout << "Resizing to: " << width << "x" << height << std::endl;
        gameState.screenWidth = width;
        gameState.screenHeight = height;
        SDL_SetWindowSize(gameState.window, width, height);
        SDL_RenderSetLogicalSize(gameState.renderer, width, height);
    }

    EMSCRIPTEN_KEEPALIVE
    void clear_canvas() {
        SDL_SetRenderTarget(gameState.renderer, gameState.drawingTexture);
        SDL_SetRenderDrawColor(gameState.renderer, 0, 0, 0, 255);
        SDL_RenderClear(gameState.renderer);
        SDL_SetRenderTarget(gameState.renderer, NULL);
    }
}

void game_loop(void* arg) {
    GameState* state = static_cast<GameState*>(arg);
    SDL_Event event;


    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_MOUSEMOTION:
//                std::cout << "Mouse Motion - Pressed: " << (state->mousePressed ? "true" : "false") 
//                         << " X: " << event.motion.x 
//                         << " Y: " << event.motion.y << std::endl;
                if (state->mousePressed) {
                    SDL_SetRenderTarget(state->renderer, state->drawingTexture);
                    SDL_SetRenderDrawColor(state->renderer, 255, 255, 255, 255);
                    SDL_RenderDrawLine(state->renderer, state->lastMouseX,
                            state->lastMouseY, event.motion.x, event.motion.y);
                    SDL_SetRenderTarget(state->renderer, NULL);

                    state->lastMouseX = event.motion.x;
                    state->lastMouseY = event.motion.y;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    std::cout << "mouse pressed down" << std::endl;
                    state->mousePressed = true;
                    state->lastMouseX = event.motion.x;
                    state->lastMouseY = event.motion.y;
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

    SDL_SetRenderDrawColor(state->renderer, 255, 255, 255, 255);
    SDL_RenderClear(state->renderer);
    SDL_RenderCopy(state->renderer, state->drawingTexture, NULL, NULL);
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

    gameState.drawingTexture = SDL_CreateTexture(
        gameState.renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        width,
        height
    );

    SDL_SetRenderTarget(gameState.renderer, gameState.drawingTexture);
    SDL_SetRenderDrawColor(gameState.renderer, 0, 0, 0, 255);
    SDL_RenderClear(gameState.renderer);
    SDL_SetRenderTarget(gameState.renderer, NULL);

    SDL_SetWindowTitle(gameState.window, "web game");


#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(emscripten_loop, &gameState, 0, 1);
#else
    while (gameState.running) {
        game_loop(&gameState);
    }
#endif

    if (gameState.drawingTexture) {
        SDL_DestroyTexture(gameState.drawingTexture);
    }
    SDL_DestroyRenderer(gameState.renderer);
    SDL_DestroyWindow(gameState.window);
    SDL_Quit();

    return 0;
}
