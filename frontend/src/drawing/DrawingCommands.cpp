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

void addDrawCommandInternal(int startX, int startY, int endX, int endY, int type) {
    drawStartX.push_back(startX);
    drawStartY.push_back(startY);
    drawEndX.push_back(endX);
    drawEndY.push_back(endY);
    drawTypes.push_back(type);
    ++drawCommandCount;
}

void addMouseDownCommandInternal() {
    drawStartX.push_back(-1);
    drawStartY.push_back(-1);
    drawEndX.push_back(-1);
    drawEndY.push_back(-1);
    drawTypes.push_back(MOUSEDOWN);
    ++drawCommandCount;
}

int getDrawCommandCountInternal() {
    return drawCommandCount;
}

int getNewCommandCountInternal() {
    return drawCommandCount - lastSentIndex;
}

void markCommandsSentInternal() {
    lastSentIndex = drawCommandCount;
}

int* getStartXArrayInternal() {
    return drawStartX.data();
}

int* getStartYArrayInternal() {
    return drawStartY.data();
}

int* getEndXArrayInternal() {
    return drawEndX.data();
}

int* getEndYArrayInternal() {
    return drawEndY.data();
}

int* getDrawTypesInternal() {
    return drawTypes.data();
}

void clearDrawCommandsInternal() {
    drawStartX.clear();
    drawStartY.clear();
    drawEndX.clear();
    drawEndY.clear();
    drawTypes.clear();
    drawCommandCount = 0;
    lastSentIndex = 0;
}
