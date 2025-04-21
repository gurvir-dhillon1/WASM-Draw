// src/main.cpp
#include <SDL.h>
#include <iostream>
#include "game/GameState.h"
#include "game/GameLoop.h"
#include "drawing/DrawingCommands.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

// Function declarations
void initializeSDL();
void cleanup();

int main(int argc, char* argv[]) {
    // Initialize SDL
    initializeSDL();

    // Default window dimensions
    int width = 800;
    int height = 600;

    // Initialize the game state
    if (!initializeGameState(&gameState, width, height)) {
        std::cerr << "Failed to initialize game state!" << std::endl;
        cleanup();
        return 1;
    }

    // Set up the main loop
#ifdef __EMSCRIPTEN__
    // When targeting WebAssembly, use the emscripten main loop
    emscripten_set_main_loop_arg(emscriptenLoop, &gameState, 0, 1);
#else
    // Native platform main loop
    while (gameState.running) {
        gameLoop(&gameState);
        SDL_Delay(16); // ~60 fps
    }
#endif

    // Cleanup resources
    cleanup();
    return 0;
}

void initializeSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        exit(1);
    }
}

void cleanup() {
    cleanupGameState(&gameState);
    SDL_Quit();
}

#ifdef __EMSCRIPTEN__
// Additional WebAssembly-specific functions can be added here
#endif
