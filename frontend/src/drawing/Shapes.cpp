// src/drawing/Shapes.cpp
#include "drawing/Shapes.h"
#include "drawing/DrawingCommands.h"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#ifdef __EMSCRIPTEN__
extern "C" {
#endif
EMSCRIPTEN_KEEPALIVE
void drawLine(
    int startX,
    int startY,
    int endX,
    int endY,
    int add_to_stack
) {
    SDL_SetRenderTarget(gameState.renderer, gameState.drawingTexture);
    SDL_SetRenderDrawColor(gameState.renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(gameState.renderer, startX, startY, endX, endY);
    SDL_SetRenderTarget(gameState.renderer, NULL);
    if (add_to_stack)
        addDrawCommand(startX, startY, endX, endY, LINE);
}

EMSCRIPTEN_KEEPALIVE
void erase(
    int startX,
    int startY,
    int endX,
    int endY,
    int add_to_stack
) {
    SDL_SetRenderTarget(gameState.renderer, gameState.drawingTexture);
    SDL_SetRenderDrawColor(gameState.renderer, 0, 0, 0, 255);
    
    // Define eraser size (width and height of rectangle)
    const int eraserSize = 20;
    const int halfSize = eraserSize / 2;
    
    // For smooth erasing, we'll interpolate points between start and end
    float distance = sqrt(pow(endX - startX, 2) + pow(endY - startY, 2));
    int steps = (distance < 1) ? 1 : (int)distance * 2; // Double the distance for more density
    
    for (int i = 0; i <= steps; i++) {
        float t = (steps == 0) ? 0.0f : (float)i / (float)steps;
        int x = startX + (endX - startX) * t;
        int y = startY + (endY - startY) * t;
        
        // Create and draw a filled rectangle centered at the current point
        SDL_Rect rect = {x - halfSize, y - halfSize, eraserSize, eraserSize};
        SDL_RenderFillRect(gameState.renderer, &rect);
    }
    
    SDL_SetRenderTarget(gameState.renderer, NULL);
    if (add_to_stack)
        addDrawCommand(startX, startY, endX, endY, ERASE);
}

void drawEntireStack() {
    for (int i = 0; i < drawStartX.size(); ++i) {
        switch (drawTypes[i]) {
            case LINE:
                drawLine(drawStartX[i], drawStartY[i], drawEndX[i], drawEndY[i], false);
                break;
            case ERASE:
                erase(drawStartX[i], drawStartY[i], drawEndX[i], drawEndY[i], false);
                break;
            default:
                break;
        }
    }
}
#ifdef __EMSCRIPTEN__
}
#endif
