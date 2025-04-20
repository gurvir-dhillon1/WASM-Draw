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
extern std::vector<int> g_drawStartX;
extern std::vector<int> g_drawStartY;
extern std::vector<int> g_drawEndX;
extern std::vector<int> g_drawEndY;
extern std::vector<int> g_drawTypes;
extern int g_drawCommandCount;
extern int g_lastSentIndex;

// Functions for managing drawing commands
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
