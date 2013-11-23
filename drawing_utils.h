// ArduinoCopter
// drawing_utils.h
//
// Created November 22, 2013
//
// Utility functions for drawing to the display.

#ifndef __drawing_utils_h__
#define __drawing_utils_h__
#include "geometry.h"
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <Adafruit_GFX.h>    // Core graphics library

// Draws a rectangle specified using a `g_rect` struct.
//
// @param tft	Pointer to the TFT display struct.
// @param rect 	The rectangle to draw.
// @param color	The color to use to fill the rect.
void draw_rect(Adafruit_ST7735 *tft, g_rect rect, int color);

#endif
