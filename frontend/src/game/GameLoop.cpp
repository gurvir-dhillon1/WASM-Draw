// src/game/GameLoop.cpp
#include "game/GameLoop.h"
#include "game/GameState.h"
#include "drawing/Shapes.h"
#include "drawing/DrawingCommands.h"
#include <iostream>

void gameLoop(void* arg) {
    GameState* state = static_cast<GameState*>(arg);
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_MOUSEMOTION:
                if (state->mousePressed) {
                    if (getLineMode(state)) {
                        SDL_SetRenderTarget(state->renderer, NULL);
                        SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, 255);
                        SDL_RenderClear(state->renderer);

                        SDL_RenderCopy(state->renderer, state->drawingTexture, NULL, NULL);

                        SDL_SetRenderDrawColor(state->renderer, 255, 255, 255, 255);
                        SDL_RenderDrawLine(state->renderer, state->lastMouseX, state->lastMouseY,
                                event.motion.x, event.motion.y);
                        SDL_RenderPresent(state->renderer);
                    } 
                    else if (getCircleMode(state)) {
                        SDL_SetRenderTarget(state->renderer, NULL);
                        SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, 255);
                        SDL_RenderClear(state->renderer);

                        SDL_RenderCopy(state->renderer, state->drawingTexture, NULL, NULL);

                        SDL_SetRenderDrawColor(state->renderer, 255, 255, 255, 255);

                        int centerX = state->lastMouseX;
                        int centerY = state->lastMouseY;
                        int radius = sqrt(pow(event.motion.x - centerX, 2) + pow(event.motion.y - centerY, 2));
                        drawCircle(state->renderer, centerX, centerY, radius);

                        SDL_RenderPresent(state->renderer);
                    } 
                    else if (getSquareMode(state)) {
                        SDL_SetRenderTarget(state->renderer, NULL);
                        SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, 255);
                        SDL_RenderClear(state->renderer);

                        SDL_RenderCopy(state->renderer, state->drawingTexture, NULL, NULL);

                        SDL_SetRenderDrawColor(state->renderer, 255, 255, 255, 255);
                        drawSquare(state->renderer, state->lastMouseX, state->lastMouseY, 
                                  event.motion.x, event.motion.y);

                        SDL_RenderPresent(state->renderer);
                    }
                    else {
                        drawLine( 
                                state->lastMouseX,
                                state->lastMouseY, 
                                event.motion.x,
                                event.motion.y,
                                FREE,
                                1
                        );

                        setLastMousePosition(state, event.motion.x, event.motion.y);
                    }
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (state->mousePressed == false) 
                    addMouseDownCommand();
                if (event.button.button == SDL_BUTTON_LEFT) {
                    setMousePressed(state, true);
                    setLastMousePosition(state, event.motion.x, event.motion.y);
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    setMousePressed(state, false);
                    if (getLineMode(state)) {
                        drawLine(
                            state->lastMouseX,
                            state->lastMouseY, 
                            event.motion.x,
                            event.motion.y,
                            LINE,
                            1
                        );
                        setLineMode(state); // Toggle off
                    } 
                    else if (getCircleMode(state)) {
                        SDL_SetRenderTarget(state->renderer, state->drawingTexture);
                        SDL_SetRenderDrawColor(state->renderer, 255, 255, 255, 255);
                        int centerX = state->lastMouseX;
                        int centerY = state->lastMouseY;
                        int radius = sqrt(pow(event.motion.x - centerX, 2) + pow(event.motion.y - centerY, 2));
                        drawCircle(state->renderer, centerX, centerY, radius);
                        SDL_SetRenderTarget(state->renderer, NULL);
                        addDrawCommand(centerX, centerY, radius, 0, CIRCLE);
                        setCircleMode(state); // Toggle off
                    } 
                    else if (getSquareMode(state)) {
                        SDL_SetRenderTarget(state->renderer, state->drawingTexture);
                        SDL_SetRenderDrawColor(state->renderer, 255, 255, 255, 255);
                        drawSquare(state->renderer, state->lastMouseX, state->lastMouseY, 
                                  event.motion.x, event.motion.y);
                        SDL_SetRenderTarget(state->renderer, NULL);
                        addDrawCommand(state->lastMouseX, state->lastMouseY, 
                                                        event.motion.x, event.motion.y, RECT);
                        setSquareMode(state); // Toggle off
                    }
                }
                break;
            case SDL_QUIT:
                setRunning(state, false);
                break;
        }
    } 

    SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, 255);
    SDL_RenderClear(state->renderer);
    SDL_RenderCopy(state->renderer, state->drawingTexture, NULL, NULL);
    if (!(getLineMode(state) || getCircleMode(state) || getSquareMode(state))) {
        SDL_RenderPresent(state->renderer);
    }
}

#ifdef __EMSCRIPTEN__
void emscriptenLoop(void* arg) {
    gameLoop(arg);
}
#endif
