// src/drawing/DrawingCommandsBindings.cpp
#include "drawing/DrawingCommandsBindings.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#ifdef __EMSCRIPTEN__
extern "C" {
#endif

EMSCRIPTEN_KEEPALIVE
void addDrawCommand(int startX, int startY, int endX, int endY, int type) {
    addDrawCommandInternal(startX, startY, endX, endY, type);
}

EMSCRIPTEN_KEEPALIVE
int getDrawCommandCount() {
    return getDrawCommandCountInternal();
}

EMSCRIPTEN_KEEPALIVE
int getNewCommandCount() {
    return getNewCommandCountInternal();
}

EMSCRIPTEN_KEEPALIVE
void markCommandsSent() {
    return markCommandsSentInternal();
}

EMSCRIPTEN_KEEPALIVE
int* getStartXArray() {
    return getStartXArrayInternal();
}

EMSCRIPTEN_KEEPALIVE
int* getStartYArray() {
    return getStartYArrayInternal();
}

EMSCRIPTEN_KEEPALIVE
int* getEndXArray() {
    return getEndXArrayInternal();
}

EMSCRIPTEN_KEEPALIVE
int* getEndYArray() {
    return getEndYArrayInternal();
}

EMSCRIPTEN_KEEPALIVE
int* getDrawTypes() {
    return getDrawTypesInternal();
}

EMSCRIPTEN_KEEPALIVE
void clearDrawCommands() {
    clearDrawCommandsInternal();
}

#ifdef __EMSCRIPTEN__
}
#endif

