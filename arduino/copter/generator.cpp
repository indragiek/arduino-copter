// ArduinoCopter
// generator.cpp
//
// Created November 19, 2013

#include <Arduino.h>
#include "generator.h"

// Generates and returns a new frame.
//
// @param g Pointer to the generator.
//
// @return The newly created frame.
static gen_frame gen_generate_next_frame(generator *g);

// Detects whether an object inside a rectangle specified in screen
// coordinates is colliding with the terrain boundaries.
//
// @param g Pointer to the generator.
// @param x The x coordinate of the frame in screen coordinates.
// @param y The y coordinate of the object to test collisions for.
// @param h The height of the object to test collisions for.
//
// @return Boolean value indicating whether a collision was detected.
static boolean gen_detect_frame_collision(generator *g, int x, int y, int h);

// =========== Public API ============
// All Public APIs are documented in generator.h.

generator * gen_new(g_size size, int spacing, int max_d) {
    generator *g = (generator *)malloc(sizeof(generator));
    g->size = size;
    g->spacing = spacing;
    g->max_delta = max_d;
    g->num_frames = 0;
    g->frames = (gen_frame *)malloc(size.width * sizeof(gen_frame));

    for (int i = 0; i < size.width; i++) {
        g->frames[i] = gen_generate_next_frame(g);
        g->num_frames++;
    }

    return g;
}

gen_frame gen_pop_frame(generator *g, gen_frame *new_frame) {
    // Pop the left most frame and generate a new frame to append
    // to the end of the generator's frame list.
    gen_frame *frames = g->frames;
    gen_frame f = frames[0];
    size_t len = g->num_frames;
    for (int i = 1; i < len; i++) {
        frames[i - 1] = frames[i];
    }
    gen_frame f_new = gen_generate_next_frame(g);
    frames[len - 1] = f_new;
    if (new_frame) *new_frame = f_new;
    return f;
}

boolean gen_detect_collision(generator *g, g_rect r) {
    for (int x = r.origin.x; x < r.size.width; x++) {
        if (gen_detect_frame_collision(g, x, r.origin.y, r.size.height)) {
            return true;
        }
    }
    return false;
}

// =========== Private API ============

static gen_frame gen_generate_next_frame(generator *g) {
    gen_frame f;
    size_t len = g->num_frames;
    if (len > 0) {
        f = g->frames[len - 1];
    } else {
        // If this is the first frame in the generator, start it off at the
        // "median" position, ie. equivalent sized boundaries on top and bottom.
        int half_max = (g->size.height - g->spacing) / 2;
        f.top_height = half_max;
        f.bottom_height = half_max;
    }

    int max_d = g->max_delta;
    int d = random(max(-f.top_height, -max_d), min(f.bottom_height, max_d) + 1);
    f.top_height += d;
    f.bottom_height -= d;
    return f;
}

static boolean gen_detect_frame_collision(generator *g, int x, int y, int h) {
    gen_frame f = g->frames[x];
    return (y <= f.top_height) || ((y + h) >= (g->size.height - f.bottom_height));
}
