// include/drawing/Shapes.h
#pragma once

#include <SDL.h>

// Drawing functions for shapes
void drawLine(
        SDL_Renderer* renderer,
        SDL_Texture* targetTexture,
        int startX,
        int startY,
        int endX,
        int endY,
        int type,
        bool add_to_stack
);
void drawCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius);
void drawSquare(SDL_Renderer* renderer, int startX, int startY, int endX, int endY);
