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
    .lineMode = 0,
    .circleMode = 0,
    .squareMode = 0,
    .screenWidth = 800,
    .screenHeight = 600,
    .lastMouseX = -1,
    .lastMouseY = -1
};

bool initializeGameState(GameState* state, int width, int height) {
    state->running = true;
    state->screenWidth = width;
    state->screenHeight = height;
    
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
EMSCRIPTEN_KEEPALIVE
bool resizeRenderer(GameState* state, int width, int height) {
    std::cout << "Resizing to: " << width << "x" << height << std::endl;
    SDL_Texture* oldTexture = state->drawingTexture;
    int oldWidth, oldHeight;
    SDL_QueryTexture(oldTexture, NULL, NULL, &oldWidth, &oldHeight);

    state->drawingTexture = SDL_CreateTexture(
        state->renderer,
        SDL_PIXELFORMAT_ABGR8888,
        SDL_TEXTUREACCESS_TARGET,
        width,
        height
    );

    if (!state->drawingTexture) {
        std::cerr << "Failed to create new texture during resize" << std::endl;
        return false;
    }

    SDL_SetRenderTarget(state->renderer, state->drawingTexture);
    SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, 255);
    SDL_RenderClear(state->renderer);

    SDL_Rect SrcRect = {0, 0, oldWidth, oldHeight};
    SDL_Rect DstRect = {0, 0, oldWidth, oldHeight};
    SDL_RenderCopy(state->renderer, oldTexture, &SrcRect, &DstRect);
    SDL_SetRenderTarget(state->renderer, NULL);

    SDL_DestroyTexture(oldTexture);

    state->screenWidth = width;
    state->screenHeight = height;

    SDL_SetWindowSize(state->window, width, height);
    SDL_RenderSetLogicalSize(state->renderer, width, height);
    
    return true;
}

EMSCRIPTEN_KEEPALIVE
void clearCanvas(GameState* state) {
    SDL_SetRenderTarget(state->renderer, state->drawingTexture);
    SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, 255);
    SDL_RenderClear(state->renderer);
    SDL_SetRenderTarget(state->renderer, NULL);
    clearDrawCommands();
}

EMSCRIPTEN_KEEPALIVE
int setLineMode(GameState* state) {
    state->lineMode = !state->lineMode;
    state->circleMode = 0;
    state->squareMode = 0;
    return state->lineMode;
}

EMSCRIPTEN_KEEPALIVE
int getLineMode(GameState* state) {
    return state->lineMode;
}

EMSCRIPTEN_KEEPALIVE
int setCircleMode(GameState* state) {
    state->lineMode = 0;
    state->circleMode = !state->circleMode;
    state->squareMode = 0;
    return state->circleMode;
}

EMSCRIPTEN_KEEPALIVE
int getCircleMode(GameState* state) {
    return state->circleMode;
}

EMSCRIPTEN_KEEPALIVE
int setSquareMode(GameState* state) {
    state->lineMode = 0;
    state->circleMode = 0;
    state->squareMode = !state->squareMode;
    return state->squareMode;
}

EMSCRIPTEN_KEEPALIVE
int getSquareMode(GameState* state) {
    return state->squareMode;
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
