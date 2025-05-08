// include/drawing/DrawingCommands.h
#pragma once

#include <vector>

// Make sure this aligns with the ENUM in ~/backend/server.go
enum Draw_Modes {
    FREE,
    LINE,
    CIRCLE,
    RECT,
    MOUSEDOWN
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
void addDrawCommandInternal(int startX, int startY, int endX, int endY, int type);
void addMouseDownCommandInternal();
int getDrawCommandCountInternal();
int getNewCommandCountInternal();
void markCommandsSentInternal();
int* getStartXArrayInternal();
int* getStartYArrayInternal();
int* getEndXArrayInternal();
int* getEndYArrayInternal();
int* getDrawTypesInternal();
void clearDrawCommandsInternal();
