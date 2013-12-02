// ArduinoCopter
// helicopter.cpp
//
// Created December 1, 2013
//

#include "helicopter.h"
#include "drawing_utils.h"

// =========== Function Declarations ============

// Draw a copter pixel adjusted for the current copter origin.
//
// @param tft       Pointer to the TFT in which to draw the copter.
// @param origin	The origin of the helicopter sprite.
// @param p         The point of the pixel to draw.
// @param color     The color to fill the pixel with.
static void helicopter_draw_pixel(Adafruit_GFX *tft, g_point origin, g_point p, int color);

// =========== Constants ============

// Pixel size of the copter.
const g_size helicopter_size = {11, 6};

// Array of pixels to use for drawing the copter body at an assumed
// origin of {0, 0}
static const g_point body_pixels[] =  {{7, 1},  {2, 2}, {6, 2}, {7, 2}, {8, 2}, {1, 3}, {2, 3}, {3, 3}, {4, 3}, {5, 3}, {6, 3}, {7, 3}, {8, 3}, {9, 3}, {2, 4}, {5, 4}, {6, 4}, {7, 4}, {8, 4}, {9, 4}, {6, 5}, {7, 5}, {8, 5}};

// Start and end X positions of the copter blade (used for animating the blade)
static const g_point blade_start = {4, 0};
static const g_point blade_end = {10, 0};

// The number of frames before the direction of the blade switches when
// the copter is animating.
static const int animation_frame_count = 1;

// =========== Public API ============
// All Public APIs are documented in helicopter.h

void helicopter_draw(Adafruit_GFX *tft, g_point origin, int color) {
    int num_pixels = sizeof(body_pixels) / sizeof(g_point);
    for (int i = 0; i < num_pixels; i++) {
        helicopter_draw_pixel(tft, origin, body_pixels[i], color);
    }

    const int half_blade = (blade_end.x - blade_start.x) / 2;
    const int blade_y = blade_start.y;
    static int current_frame_count = 0;
    static boolean left_blade = true;

    if (++current_frame_count >= animation_frame_count) {
        current_frame_count = 0;
        left_blade = !left_blade;
    }
    int start_x = blade_start.x;
    if (left_blade == false) {
        start_x += half_blade;
    }
    for (int i = start_x; i < (start_x + half_blade + 1); i++) {
        helicopter_draw_pixel(tft, origin, (g_point){i, blade_y}, color);
    }
}

static void helicopter_draw_pixel(Adafruit_GFX *tft, g_point origin, g_point p, int color) {
    p.x += origin.x;
    p.y += origin.y;
    draw_pixel(tft, p, color);
}
