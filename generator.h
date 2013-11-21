// ArduinoCopter
// generator.h
//
// Created November 19, 2013
//
// A terrain generator for the ArduinoCopter game that generates random
// terrains for the top and bottom edges of the tunnel. The generated 
// terrains are created using the Arduino's randomSeed() and random()
// functions with configurable spacing and height deltas.
//

#ifndef __generator_h__
#define __generator_h__

#include "ll.h"
#include "geometry.h"

typedef struct {
	linked_list *top;
	linked_list *bottom;
	g_size size;
	int spacing;
	int max_delta;
} generator;

//
// A `gen_frame` (generator frame) constitutes a single "frame" of the
// randomly generated terrain sequence. A frame represents a section of
// the terrain that is 1 pixel wide. Each frame contains the `x` coordinate
// of where the frame is located, and the heights of the top and bottom
// boundaries. These values are used directly to draw the frame as a pair
// of 2 rectangles on screen.
//
typedef struct {
	int x;
	int top_height;
	int bottom_height;
} gen_frame;

// Create a new generator and generates the first set of frames.
//
// @param size 		g_size structure containing the pixel width and height of the
//					region for which the generator is creating frames for.
// @param spacing	The fixed spacing between the top and bottom boundaries.
//					In other words, the sum of the heights of the bottom and 
//					top boundaries will always be equal to height - spacing.
// @param max_d 	The maximum variation in height between one frame and the next.
//
// @return A pointer to the newly created `generator` struct.
//
generator * gen_new(g_size size, int spacing, int max_d);

// Pops the first frame in the generator and returns it. Generates a new frame
// and appends it to the end of the generator's frames list in order to replace
// the one that was popped.
//
// @param g 		Pointer to the generator.
// @param new_frame	Pointer to be set to the newly created frame (appended to right).
//
// @return The popped generator frame.
gen_frame gen_pop_frame(generator *g, gen_frame *new_frame);

// Copies all the frames of the generator in its current state and returns
// them as an array of gen_frame structs. The caller is responsible for freeing
// the array afterwards.
//
// @param g 	Pointer to the generator.
// @param len	Pointer to a `size_t` to be set to the length of the returned array.
//
// @return An array of `gen_frame` structs.
gen_frame * gen_copy_frames(generator *g, size_t *len);

// Detects a collision between an object located in an arbitrary rectangle and
// the top or bottom boundaries of the terrain. 
//
// @param g Pointer to the generator.
// @param r The rectangle for which to test collisions for. The origin of the
//			is relative to the screen coordinates, meaning that the origin {0,0}
//			is located on the top left corner of the region.
//
// @return 	true if a collision occurred, false otherwise.
boolean gen_detect_collision(generator *g, g_rect r);

// Frees the generator and all associated memory.
//
// @param g Pointer to the generator.
//
void gen_free(generator *g);

#endif
