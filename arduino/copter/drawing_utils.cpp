// ArduinoCopter
// drawing_utils.cpp
//
// Created November 22, 2013
//

#include "drawing_utils.h"

void draw_rect(Adafruit_GFX *tft, g_rect rect, int color) {
	const int x = rect.origin.x;
	const int y = rect.origin.y;
	const int w = rect.size.width;
	const int h = rect.size.height;

	boolean w_unit = w == 1;
	boolean h_unit = h == 1;

	if (w_unit == true && h_unit == true) {
		draw_pixel(tft, rect.origin, color);
	} else if (w_unit == true) {
		tft->drawFastVLine(x, y, h, color);
	} else if (h_unit == true) {
		tft->drawFastHLine(x, y, w, color);
	} else {
		tft->fillRect(x, y, w, h, color);
	}
}

void draw_pixel(Adafruit_GFX *tft, g_point point, int color) {
	tft->drawPixel(point.x, point.y, color);
}
