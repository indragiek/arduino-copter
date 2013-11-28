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
#include "geometry.h"

typedef struct {
	int background; // Color of the background of the game.
	int terrain;	// Color of the terrain on the top and bottom.
	int blocks;		// Color of the obstable blocks.
} scene_colors;

typedef struct {
    Adafruit_ST7735 *tft; 	// Display being drawn into.
    generator *gen;			// Terrain generator.
    gen_frame *frames;		// Array of generator frames visible on screen.
    size_t num_frames;		// Length of `frames` array.
    g_rect *block_rects;	// Array of block rectangles for the obstacle blocks.
    size_t num_blocks;		// Number of blocks present (or upcoming) on screen.
    int last_block_d;		// Distance passed since the last block was inserted.
    int max_block_d;		// Distance between obstacle blocks.
    g_size block_size;		// Size of obstacle blocks.
    scene_colors colors;	// Color definitions.
} scene;

typedef enum {
    copter_up = 0,
    copter_down = 1
} copter_direction;

// Creates a new scene.
//
// @param tft       Pointer to the TFT display to draw the scene into.
// @param spacing   The fixed spacing between the top and bottom boundaries.
//                  In other words, the sum of the heights of the bottom and 
//                  top boundaries will always be equal to height - spacing.
// @param max_d     The maximum variation in height between one frame and the next.
// @param blk_d		Distance between obstable blocks.
// @param blk_size	Size of obstacle blocks.
// @param colors 	`scene_color` struct containing the colors used for drawing the
//					scene (background, terrain, etc.)
//
// @return A pointer to the newly created `scene` struct.
//
scene * scene_new(Adafruit_ST7735 *tft, 
	              int spacing, 
	              int max_d, 
	              int blk_d, 
	              g_size blk_size,
	              scene_colors colors);

// Updates the scene by drawing the next frame.
//
// @param s     Pointer to the `scene` structure to update.
// @param dir   Movement direction of the helicopter (`copter_up` or `copter_down`).
//
void scene_update(scene *s, copter_direction dir);

// Frees all memory associated with the scene.
//
// @param s Pointer to the `scene` to free.
//
void scene_free(scene *s);

#endif