// src/drawing/DrawingCommands.cpp
#include "drawing/DrawingCommands.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

// Global variables
std::vector<int> g_drawStartX;
std::vector<int> g_drawStartY;
std::vector<int> g_drawEndX;
std::vector<int> g_drawEndY;
std::vector<int> g_drawTypes;
int g_drawCommandCount = 0;
int g_lastSentIndex = 0;

#ifdef __EMSCRIPTEN__
extern "C" {
#endif

EMSCRIPTEN_KEEPALIVE
void add_draw_command(int startX, int startY, int endX, int endY, int type) {
    g_drawStartX.push_back(startX);
    g_drawStartY.push_back(startY);
    g_drawEndX.push_back(endX);
    g_drawEndY.push_back(endY);
    g_drawTypes.push_back(type);
    ++g_drawCommandCount;
}

EMSCRIPTEN_KEEPALIVE
int get_draw_command_count() {
    return g_drawCommandCount;
}

EMSCRIPTEN_KEEPALIVE
int get_new_command_count() {
    return g_drawCommandCount - g_lastSentIndex;
}

EMSCRIPTEN_KEEPALIVE
void mark_commands_sent() {
    g_lastSentIndex = g_drawCommandCount;
}

EMSCRIPTEN_KEEPALIVE
int* get_start_x_array() {
    return g_drawStartX.data();
}

EMSCRIPTEN_KEEPALIVE
int* get_start_y_array() {
    return g_drawStartY.data();
}

EMSCRIPTEN_KEEPALIVE
int* get_end_x_array() {
    return g_drawEndX.data();
}

EMSCRIPTEN_KEEPALIVE
int* get_end_y_array() {
    return g_drawEndY.data();
}

EMSCRIPTEN_KEEPALIVE
int* get_draw_types() {
    return g_drawTypes.data();
}

EMSCRIPTEN_KEEPALIVE
void clear_draw_commands() {
    g_drawStartX.clear();
    g_drawStartY.clear();
    g_drawEndX.clear();
    g_drawEndY.clear();
    g_drawTypes.clear();
    g_drawCommandCount = 0;
    g_lastSentIndex = 0;
}

#ifdef __EMSCRIPTEN__
}
#endif
