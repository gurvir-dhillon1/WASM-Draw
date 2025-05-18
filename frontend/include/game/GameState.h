// include/game/GameState.h
#pragma once
#include <SDL.h>

#ifdef __EMSCRIPTEN__
#include "drawing/DrawingCommandsBindings.h"
#endif

struct GameState {
    bool running;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* drawingTexture;

    bool mousePressed;

    int drawMode;

    int screenWidth;
    int screenHeight;

    int lastMouseX;
    int lastMouseY;
};

extern GameState gameState;

// Functions for game state management
bool initializeGameState(GameState* state, int width, int height);
void cleanupGameState(GameState* state);

#ifdef __EMSCRIPTEN__
extern "C" {
#endif
// Window and rendering functions
bool resizeRenderer(int width, int height);

void clearCanvas();

// Drawing Mode Functions
int getDrawMode();
void setDrawMode(int drawMode);

// Setter Functions
void setLastMousePosition(GameState* state, int lastMouseX, int lastMouseY);
void setMousePressed(GameState* state, bool mouseStatus);
void setRunning(GameState* state, bool runningStatus);
#ifdef __EMSCRIPTEN__
}
#endif
