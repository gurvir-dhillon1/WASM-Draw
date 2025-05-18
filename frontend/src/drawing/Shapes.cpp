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

#ifdef __EMSCRIPTEN__
}
#endif
