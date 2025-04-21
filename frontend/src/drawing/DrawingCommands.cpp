// src/drawing/DrawingCommands.cpp
#include "drawing/DrawingCommands.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

// Global variables
std::vector<int> drawStartX;
std::vector<int> drawStartY;
std::vector<int> drawEndX;
std::vector<int> drawEndY;
std::vector<int> drawTypes;
int drawCommandCount = 0;
int lastSentIndex = 0;

#ifdef __EMSCRIPTEN__
extern "C" {
#endif

EMSCRIPTEN_KEEPALIVE
void addDrawCommand(int startX, int startY, int endX, int endY, int type) {
    drawStartX.push_back(startX);
    drawStartY.push_back(startY);
    drawEndX.push_back(endX);
    drawEndY.push_back(endY);
    drawTypes.push_back(type);
    ++drawCommandCount;
}

EMSCRIPTEN_KEEPALIVE
int getDrawCommandCount() {
    return drawCommandCount;
}

EMSCRIPTEN_KEEPALIVE
int getNewCommandCount() {
    return drawCommandCount - lastSentIndex;
}

EMSCRIPTEN_KEEPALIVE
void markCommandsSent() {
    lastSentIndex = drawCommandCount;
}

EMSCRIPTEN_KEEPALIVE
int* getStartXArray() {
    return drawStartX.data();
}

EMSCRIPTEN_KEEPALIVE
int* getStartYArray() {
    return drawStartY.data();
}

EMSCRIPTEN_KEEPALIVE
int* getEndXArray() {
    return drawEndX.data();
}

EMSCRIPTEN_KEEPALIVE
int* getEndYArray() {
    return drawEndY.data();
}

EMSCRIPTEN_KEEPALIVE
int* getDrawTypes() {
    return drawTypes.data();
}

EMSCRIPTEN_KEEPALIVE
void clearDrawCommands() {
    drawStartX.clear();
    drawStartY.clear();
    drawEndX.clear();
    drawEndY.clear();
    drawTypes.clear();
    drawCommandCount = 0;
    lastSentIndex = 0;
}

#ifdef __EMSCRIPTEN__
}
#endif
