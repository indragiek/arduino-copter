// ArduinoCopter
// helicopter.h
//
// Created December 1, 2013
//

#ifndef __helicopter_h__
#define __helicopter_h__

#include <Adafruit_GFX.h>
#include "geometry.h"

// The pixel size of the helicopter.
extern const g_size helicopter_size;

// Draws the helicopter sprite.
//
// @param origin 	The origin point at which to draw the helicopter.
// @param color		The color used to fill the helicopter.
void helicopter_draw(Adafruit_GFX *tft, g_point origin, int color);

#endif
