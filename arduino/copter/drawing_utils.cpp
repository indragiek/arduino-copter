// ArduinoCopter
// drawing_utils.cpp
//
// Created November 22, 2013
//

#include "drawing_utils.h"

void draw_rect(Adafruit_GFX *tft, g_rect rect, int color) {
	tft->fillRect(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height, color);
}

void draw_pixel(Adafruit_GFX *tft, g_point point, int color) {
	tft->drawPixel(point.x, point.y, color);
}
