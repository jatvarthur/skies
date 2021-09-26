#pragma once

#include "winos.h"

const int FONTW = 8;
const int FONTH = 8;

bool renderInit(HWND hwnd, int width, int height);
void render();
void renderClean();
