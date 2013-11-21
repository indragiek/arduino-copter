// ArduinoCopter
// scene.h
//
// Created November 21, 2013
//
// Scene redraw and layout and drawing. Handles drawing the scrolling terrain
// and obstacles as well as drawing the helicopter and detecting collisions.
//
// The scene is updated by calling scene_update() with the helicopter movemement 
// direction, and callback functions can be registered to handle collision events.

#ifndef __scene_h__
#define __scene_h__
#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include "generator.h"

typedef struct {
	Adafruit_ST7735 *tft;
	generator *gen;
	gen_frame *frames;
	size_t num_frames;
} scene;

typedef enum {
	copter_up = 0,
	copter_down = 1
} copter_direction;

// Creates a new scene.
//
// @param tft 		Pointer to the TFT display to draw the scene into.
// @param spacing	The fixed spacing between the top and bottom boundaries.
//					In other words, the sum of the heights of the bottom and 
//					top boundaries will always be equal to height - spacing.
// @param max_d 	The maximum variation in height between one frame and the next.
//
// @return A pointer to the newly created `scene` struct.
//
scene * scene_new(Adafruit_ST7735 *tft, int spacing, int max_d);

// Updates the scene by drawing the next frame.
//
// @param s 	Pointer to the `scene` structure to update.
// @param dir 	Movement direction of the helicopter (`copter_up` or `copter_down`).
//
void scene_update(scene *s, copter_direction dir);

#endif