// include/drawing/DrawingCommands.h
#pragma once

#include <vector>

enum Draw_Modes {
    FREE,
    LINE,
    CIRCLE,
    RECT
};

// Global variables for drawing commands
extern std::vector<int> drawStartX;
extern std::vector<int> drawStartY;
extern std::vector<int> drawEndX;
extern std::vector<int> drawEndY;
extern std::vector<int> drawTypes;
extern int drawCommandCount;
extern int lastSentIndex;

// Functions for managing drawing commands
#ifdef __EMSCRIPTEN__
extern "C" {
#endif
void addDrawCommand(int startX, int startY, int endX, int endY, int type);
int getDrawCommandCount();
int getNewCommandCount();
void markCommandsSent();
int* getStartXArray();
int* getStartYArray();
int* getEndXArray();
int* getEndYArray();
int* getDrawTypes();
void clearDrawCommands();
#ifdef __EMSCRIPTEN__
}
#endif
