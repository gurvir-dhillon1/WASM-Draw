// src/drawing/Shapes.cpp
#include "drawing/Shapes.h"
#include "drawing/DrawingCommands.h"

void drawLine(
        int startX,
        int startY,
        int endX,
        int endY,
        int type,
        int add_to_stack
) {
    SDL_SetRenderTarget(gameState.renderer, gameState.drawingTexture);
    SDL_SetRenderDrawColor(gameState.renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(gameState.renderer, startX, startY, endX, endY);
    SDL_SetRenderTarget(gameState.renderer, NULL);
    if (add_to_stack)
        addDrawCommand(startX, startY, endX, endY, type);
}
void drawCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius) {
    int x = radius;
    int y = 0;
    int d = 1 - radius;

    while (x >= y) {
        SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
        SDL_RenderDrawPoint(renderer, centerX + y, centerY + x);
        SDL_RenderDrawPoint(renderer, centerX - y, centerY + x);
        SDL_RenderDrawPoint(renderer, centerX - x, centerY + y);
        SDL_RenderDrawPoint(renderer, centerX - x, centerY - y);
        SDL_RenderDrawPoint(renderer, centerX - y, centerY - x);
        SDL_RenderDrawPoint(renderer, centerX + y, centerY - x);
        SDL_RenderDrawPoint(renderer, centerX + x, centerY - y);

        y += 1;

        if (d <= 0) {
            d += 2 * y + 1;
        } else {
            x -= 1;
            d += 2 * (y - x) + 1;
        }
    }
}

void drawSquare(SDL_Renderer* renderer, int startX, int startY, int endX, int endY) {
    int dx = endX - startX;
    int dy = endY - startY;

    int sideLength = std::max(abs(dx), abs(dy));

    // Maintain the original startX, startY as the anchor point
    // and adjust the end points to form a square
    int x2, y2;
    
    // Determine the direction of drag and adjust accordingly
    if (dx >= 0 && dy >= 0) {        // Dragging right and down
        x2 = startX + sideLength;
        y2 = startY + sideLength;
    } else if (dx >= 0 && dy < 0) {  // Dragging right and up
        x2 = startX + sideLength;
        y2 = startY - sideLength;
    } else if (dx < 0 && dy >= 0) {  // Dragging left and down
        x2 = startX - sideLength;
        y2 = startY + sideLength;
    } else {                         // Dragging left and up
        x2 = startX - sideLength;
        y2 = startY - sideLength;
    }

    // Draw the square
    SDL_RenderDrawLine(renderer, startX, startY, x2, startY);     // Top edge
    SDL_RenderDrawLine(renderer, x2, startY, x2, y2);            // Right edge
    SDL_RenderDrawLine(renderer, x2, y2, startX, y2);            // Bottom edge
    SDL_RenderDrawLine(renderer, startX, y2, startX, startY);    // Left edge
}
