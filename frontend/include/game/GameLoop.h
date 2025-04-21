// include/game/GameLoop.h
#pragma once

void gameLoop(void* arg);

#ifdef __EMSCRIPTEN__
void emscriptenLoop(void* arg);
#endif
