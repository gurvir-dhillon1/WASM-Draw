// include/game/GameState.h
#pragma once
#include <SDL.h>

struct GameState {
    bool running;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* drawingTexture;

    bool mousePressed;

    int lineMode;
    int circleMode;
    int squareMode;

    int screenWidth;
    int screenHeight;

    int lastMouseX;
    int lastMouseY;
};

extern GameState gameState;

// Functions for game state management
bool initializeGameState(GameState* state, int width, int height);
void cleanupGameState(GameState* state);

// Window and rendering functions
bool resizeRenderer(GameState* state, int width, int height);
void clearCanvas(GameState* state);

// Drawing Mode Functions
int setLineMode(GameState* state);
int getLineMode(GameState* state);
int setCircleMode(GameState* state);
int getCircleMode(GameState* state);
int setSquareMode(GameState* state);
int getSquareMode(GameState* state);
