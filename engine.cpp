#include "engine.h"
#include "render.h"

bool gameInit()
{
	return true;
}

void gameStep()
{
	drawString(10, 10, "Skies .1", COLOR_BRIGHT_YELLOW, COLOR_MAGENTA);
	
	int y = 12, x = 11;
	for (int i = 1; i < 256; ++i, ++x) {
		if (i % 16 == 0) {
			y += 1;
			x = 10;
		}
		drawChar(x, y, i, COLOR_BRIGHT_GREEN, COLOR_BLUE);

	}
}

void gameClean()
{

}
