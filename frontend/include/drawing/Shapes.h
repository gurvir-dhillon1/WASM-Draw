// include/drawing/Shapes.h
#pragma once

#include <SDL.h>
#include "GameState.h"

// Drawing functions for shapes
#ifdef __EMSCRIPTEN__
#include "drawing/DrawingCommandsBindings.h"
extern "C" {
#endif
void drawLine(
    int startX,
    int startY,
    int endX,
    int endY,
    int add_to_stack
);
void erase(
    int startX,
    int startY,
    int endX,
    int endY,
    int add_to_stack
);
void drawEntireStack();
#ifdef __EMSCRIPTEN__
}
#endif
