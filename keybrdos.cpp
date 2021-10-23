#include "winos.h"
#include "keybrd.h"
#include "keybrdos.h"

// Bitmask: 
//   bit 0x01 -> key is down
//   bit 0x02 -> registers key presses
//   bit 0x40 -> key press was read
static uint8_t g_keyState[KEY_LAST] = { 0 };

#define BIT_DOWN			0x01
#define BIT_PRESSED			0x02
#define BIT_PRESSED_READ	0x04

bool keyIsDown(Key key)
{
	return g_keyState[key];
}

bool keyIsPressed(Key key)
{
	bool result = (g_keyState[key] & BIT_PRESSED) == BIT_PRESSED;
	if (result) g_keyState[key] |= BIT_PRESSED_READ;
	return result;
}

void keySetState(Key key, bool state)
{
	g_keyState[key] = state ? (BIT_PRESSED | BIT_DOWN) : 0;
}

void keyBeforeFrame()
{
}

void keyAfterFrame()
{
	for (int i = 0; i < KEY_LAST; ++i) {
		if ((g_keyState[i] & BIT_PRESSED_READ) == BIT_PRESSED_READ) {
			g_keyState[i] &= ~(BIT_PRESSED_READ | BIT_PRESSED);
		}
	}
}