// ArduinoCopter
// drawing_utils.cpp
//
// Created November 22, 2013
//

#include "drawing_utils.h"

void draw_rect(Adafruit_ST7735 *tft, g_rect rect, int color) {
	tft->fillRect(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height, color);
}
