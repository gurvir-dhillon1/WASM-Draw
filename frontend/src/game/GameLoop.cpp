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
                    if (getDrawMode() == LINE) {
                        drawLine( 
                            state->lastMouseX,
                            state->lastMouseY, 
                            event.motion.x,
                            event.motion.y,
                            1
                        );

                        setLastMousePosition(state, event.motion.x, event.motion.y);
                    } else if (getDrawMode() == ERASE) {
                        erase(
                            state->lastMouseX,
                            state->lastMouseY, 
                            event.motion.x,
                            event.motion.y,
                            1
                        );
                        setLastMousePosition(state, event.motion.x, event.motion.y);
                    }
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    setMousePressed(state, true);
                    setLastMousePosition(state, event.motion.x, event.motion.y);
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    setMousePressed(state, false);
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
    SDL_RenderPresent(state->renderer);
}

#ifdef __EMSCRIPTEN__
void emscriptenLoop(void* arg) {
    gameLoop(arg);
}
#endif
