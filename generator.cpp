// ArduinoCopter
// generator.cpp
//
// Created November 19, 2013

#include <Arduino.h>
#include "generator.h"

// Generates and appends a new frame.
//
// @param g Pointer to the generator.
//
// @return The newly created frame.
gen_frame gen_generate_next_frame(generator *g);

// Returns the number of generated frames.
//
// @param g Pointer to the generator.
int gen_length(generator *g);

// Lookup a particular generator frame without popping it.
//
// @param g Pointer to the generator.
// @param x The x position of the frame to lookup, relative to screen coords.
//
// @warning Calling this function for a nonexistent frame index will result
// in undefined behaviour.
//
// @return The generator frame.
gen_frame gen_lookup(generator *g, int x);

// Detects whether an object inside a rectangle specified in screen
// coordinates is colliding with the terrain boundaries.
//
// @param g Pointer to the generator.
// @param x The x coordinate of the frame in screen coordinates.
// @param y The y coordinate of the object to test collisions for.
// @param h The height of the object to test collisions for.
//
// @return Boolean value indicating whether a collision was detected.
boolean gen_detect_frame_collision(generator *g, int x, int y, int h);

// =========== Public API ============
// All Public APIs are documented in generator.h.

generator * gen_new(g_size size, int spacing, int max_d) {
    generator *g = (generator *)malloc(sizeof(generator));
    g->top = ll_new();
    g->bottom = ll_new();
    g->size = size;
    g->spacing = spacing;
    g->max_delta = max_d;

    for (int i = 0; i < size.width; i++) {
        gen_generate_next_frame(g);
    }

    return g;
}

gen_frame gen_pop_frame(generator *g, gen_frame *new_frame) {
    // Pop the left most frame and generate a new frame to append
    // to the end of the generator's frame list.
    gen_frame f;
    f.top_height = ll_pop_front(g->top);
    f.bottom_height = ll_pop_front(g->bottom);
    gen_frame f_new = gen_generate_next_frame(g);
    if (new_frame) *new_frame = f_new;
    return f;
}

gen_frame * gen_copy_frames(generator *g, size_t *len) {
    int g_len = gen_length(g);
    if (g_len == 0) return NULL;
    if (len) *len = g_len;

    gen_frame *a = (gen_frame *)calloc(g_len, sizeof(gen_frame));
    for (int i = 0; i < g_len; i++) {
        a[i] = gen_lookup(g, i);
    }
    return a;
}

void gen_free(generator *g) {
    ll_free(g->top);
    ll_free(g->bottom);
    free(g);
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

gen_frame gen_generate_next_frame(generator *g) {
    gen_frame f;
    int len = gen_length(g);
    if (len > 0) {
        f = gen_lookup(g, len - 1);
    } else {
        // If this is the first frame in the generator, start it off at the
        // "median" position, ie. equivalent sized boundaries on top and bottom.
        int half_max = (g->size.height - g->spacing) / 2;
        f.top_height = half_max;
        f.bottom_height = half_max;
    }

    int max_d = g->max_delta;
    int d = random(-max_d, max_d + 1);
    f.top_height += d;
    f.bottom_height -= d;
    ll_append(g->top, f.top_height);
    ll_append(g->bottom, f.bottom_height);
    return f;
}

int gen_length(generator *g) {
    return g->top->length;
}

gen_frame gen_lookup(generator *g, int x) {
    gen_frame f;
    f.top_height = ll_lookup(g->top, x);
    f.bottom_height = ll_lookup(g->bottom, x);
    return f;
}

boolean gen_detect_frame_collision(generator *g, int x, int y, int h) {
    gen_frame f = gen_lookup(g, x);
    return (y <= f.top_height) || ((y + h) >= (g->size.height - f.bottom_height));
}
