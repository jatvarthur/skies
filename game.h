#pragma once

extern int g_windowWidth;
extern int g_windowHeight;

bool gameInit();
void gameStep(int delta);
void gameClean();