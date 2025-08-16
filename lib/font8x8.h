#pragma once

extern unsigned char font8x8_basic[128][8];

/**
 * Sample the given font for given glyph,
 * x and y must fall in range [0, 7], with (0, 0) being the bottom left corner
 * and (7, 7) the top right one
 *
 * @return whether that bit is set in the bitmap or not
 */
inline bool sampleFontBitmap(unsigned char font[128][8], char glyph, int x, int y) {
	return (font[glyph][7 - y] >> x) & 1;
}