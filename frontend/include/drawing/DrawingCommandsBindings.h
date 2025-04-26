// include/drawing/DrawingCommandsBindings.h
#ifdef __EMSCRIPTEN__
#pragma once
#include "drawing/DrawingCommands.h"
#include <emscripten.h>

extern "C" {
EMSCRIPTEN_KEEPALIVE
void addDrawCommand(int startX, int startY, int endX, int endY, int type);

EMSCRIPTEN_KEEPALIVE
int getDrawCommandCount();

EMSCRIPTEN_KEEPALIVE
int getNewCommandCount();

EMSCRIPTEN_KEEPALIVE
void markCommandsSent();

EMSCRIPTEN_KEEPALIVE
int* getStartXArray();

EMSCRIPTEN_KEEPALIVE
int* getStartYArray();

EMSCRIPTEN_KEEPALIVE
int* getEndXArray();

EMSCRIPTEN_KEEPALIVE
int* getEndYArray();

EMSCRIPTEN_KEEPALIVE
int* getDrawTypes();

EMSCRIPTEN_KEEPALIVE
void clearDrawCommands();

}
#endif
