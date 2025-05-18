// src/game/GameState.cpp
#include "game/GameState.h"
#include "drawing/DrawingCommands.h"
#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

// Global Instance
GameState gameState = {
    .running = false,
    .window = nullptr,
    .renderer = nullptr,
    .drawingTexture = nullptr,
    .mousePressed = false,
    .drawMode = 0,
    .screenWidth = 800,
    .screenHeight = 600,
    .lastMouseX = -1,
    .lastMouseY = -1
};

bool initializeGameState(GameState* state, int width, int height) {
    state->running = true;
    state->screenWidth = width;
    state->screenHeight = height;
    SDL_SetHint(SDL_HINT_EMSCRIPTEN_KEYBOARD_ELEMENT, "#canvas");    
    if (SDL_CreateWindowAndRenderer(width, height, SDL_WINDOW_SHOWN, &state->window, &state->renderer) < 0) {
        std::cerr << "Failed to create window or renderer: " << SDL_GetError() << std::endl;
        return false;
    }

    state->drawingTexture = SDL_CreateTexture(
        state->renderer,
        SDL_PIXELFORMAT_ABGR8888,
        SDL_TEXTUREACCESS_TARGET,
        width,
        height
    );

    if (!state->drawingTexture) {
        std::cerr << "Failed to init drawingTexture: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_SetRenderTarget(state->renderer, state->drawingTexture);
    SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, 255);
    SDL_RenderClear(state->renderer);
    SDL_SetRenderTarget(state->renderer, NULL);

    SDL_SetWindowTitle(state->window, "wasm draw");
    
    return true;
}

void cleanupGameState(GameState* state) {
    if (state->drawingTexture) {
        SDL_DestroyTexture(state->drawingTexture);
        state->drawingTexture = nullptr;
    }
    
    if (state->renderer) {
        SDL_DestroyRenderer(state->renderer);
        state->renderer = nullptr;
    }
    
    if (state->window) {
        SDL_DestroyWindow(state->window);
        state->window = nullptr;
    }
}

#ifdef __EMSCRIPTEN__
extern "C" {
#endif

// Wrap initializeGameState inside a simple function so we can initialize the game state from JS
// This way we can initialize game state -> resize canvas on initial load
EMSCRIPTEN_KEEPALIVE
bool initialize(int width, int height) {
    return initializeGameState(&gameState, width, height);
}

EMSCRIPTEN_KEEPALIVE
bool resizeRenderer(int width, int height) {
    std::cout << "Resizing to: " << width << "x" << height << std::endl;
    SDL_Texture* oldTexture = gameState.drawingTexture;
    int oldWidth, oldHeight;
    SDL_QueryTexture(oldTexture, NULL, NULL, &oldWidth, &oldHeight);

    gameState.drawingTexture = SDL_CreateTexture(
        gameState.renderer,
        SDL_PIXELFORMAT_ABGR8888,
        SDL_TEXTUREACCESS_TARGET,
        width,
        height
    );

    if (!gameState.drawingTexture) {
        std::cerr << "Failed to create new texture during resize: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_SetRenderTarget(gameState.renderer, gameState.drawingTexture);
    SDL_SetRenderDrawColor(gameState.renderer, 0, 0, 0, 255);
    SDL_RenderClear(gameState.renderer);

    SDL_Rect SrcRect = {0, 0, oldWidth, oldHeight};
    SDL_Rect DstRect = {0, 0, oldWidth, oldHeight};
    SDL_RenderCopy(gameState.renderer, oldTexture, &SrcRect, &DstRect);
    SDL_SetRenderTarget(gameState.renderer, NULL);

    SDL_DestroyTexture(oldTexture);

    gameState.screenWidth = width;
    gameState.screenHeight = height;

    SDL_SetWindowSize(gameState.window, width, height);
    SDL_RenderSetLogicalSize(gameState.renderer, width, height);
    
    return true;
}

EMSCRIPTEN_KEEPALIVE
void clearCanvas() {
    SDL_SetRenderTarget(gameState.renderer, gameState.drawingTexture);
    SDL_SetRenderDrawColor(gameState.renderer, 0, 0, 0, 255);
    SDL_RenderClear(gameState.renderer);
    SDL_SetRenderTarget(gameState.renderer, NULL);
    clearDrawCommands();
}

EMSCRIPTEN_KEEPALIVE
void setDrawMode(GameState* state, int drawMode) {
    state->drawMode = drawMode;
    return;
}

EMSCRIPTEN_KEEPALIVE
int getDrawMode(GameState* state) {
    return state->drawMode;
}

EMSCRIPTEN_KEEPALIVE
void setLastMousePosition(GameState* state, int lastMouseX, int lastMouseY) {
    state->lastMouseX = lastMouseX;
    state->lastMouseY = lastMouseY;
}

EMSCRIPTEN_KEEPALIVE
void setMousePressed(GameState* state, bool mouseStatus) {
    state->mousePressed = mouseStatus;
}

EMSCRIPTEN_KEEPALIVE
void setRunning(GameState* state, bool runningStatus) {
    state->running = runningStatus;
}
#ifdef __EMSCRIPTEN__
}
#endif
