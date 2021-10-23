#pragma once

enum Key {
	KEY_RETURN	= 0x0D,
	KEY_F1		= 0x70, 
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,

	KEY_Q		= 0x51,
	KEY_LAST	= 0xFF
};

/**
 * Returns true, if the key is currently down, resets to false, when the key is up  
 */
bool keyIsDown(Key key);

/**
 * Returns true, if the key was down, and immediately resets pressed state to false.
 * This allows to reliably register one-time key presses between frames.
 */
bool keyIsPressed(Key key);
