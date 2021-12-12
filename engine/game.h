#pragma once

extern int g_windowWidth;
extern int g_windowHeight;

bool gameInit();
bool gameAwake();
void gameStep(float delta);
void gameShutdown();
void gameClean();
