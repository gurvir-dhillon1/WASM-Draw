// include/drawing/Shapes.h
#pragma once

#include <SDL.h>
#include "GameState.h"

// Drawing functions for shapes
#ifdef __EMSCRIPTEN__
extern "C" {
#endif
void drawLine(
        int startX,
        int startY,
        int endX,
        int endY,
        int type,
        int add_to_stack
);
void drawCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius);
void drawSquare(SDL_Renderer* renderer, int startX, int startY, int endX, int endY);
#ifdef __EMSCRIPTEN__
}
#endif
