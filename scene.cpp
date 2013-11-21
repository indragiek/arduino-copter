// ArduinoCopter
// scene.cpp
//
// Created November 21, 2013
//

#include "scene.h"

// Clears the screen and draws the initial scene.
//
// @param s Pointer to the `scene` to draw.
//
void scene_initial_draw(scene *s);

// Does a partial redraw of the scene for a new set of frames. Only
// updates the pixels that are necessary, versus doing a complete redraw.
//
// @param s             Pointer to the `scene` to redraw.
// @param new_frames    The new array of frames to draw.
//
void scene_redraw(scene *s, gen_frame *new_frames);

// =========== Macros ============

// Convenience macros for pulling colors out of a passed in `scene` struct.
#define COL_TER(s)      s->colors.terrain
#define COL_BG(s)       s->colors.background

// =========== Public API ============
// All Public APIs are documented in scene.h

scene * scene_new(Adafruit_ST7735 *tft, int spacing, int max_d, scene_colors colors) {
    // Create the scene structure and draw the initial scene.
    scene *s = (scene *)malloc(sizeof(scene));
    s->tft = tft;
    s->colors = colors;
    g_size size = {tft->width(), tft->height()};
    s->gen = gen_new(size, spacing, max_d);
    scene_initial_draw(s);
    return s;
}

void scene_update(scene *s, copter_direction dir) {
    // Pop the leftmost frame from the generator.
    gen_frame new_frame;
    gen_frame popped_frame = gen_pop_frame(s->gen, &new_frame);

    // Create a copy of the original frames array to update.
    gen_frame *old_frames = s->frames;
    size_t len = s->num_frames;
    size_t bytes = len * sizeof(gen_frame);
    gen_frame *frames = (gen_frame *)malloc(bytes);
    memcpy(frames, old_frames, bytes);

    // Shift all the other frames to the left and replace the last element
    // in the array with the newly created frame.
    for (int i = 1; i < len; i++) {
        frames[i - 1] = frames[i];
    }
    frames[len - 1] = new_frame;

    // Redraw the scene with the updated frames.
    scene_redraw(s, frames);

    // Update the frame array of the scene struct.
    free(old_frames);
    s->frames = frames;
}

void scene_free(scene *s) {
    free(s->frames);
    gen_free(s->gen);
    free(s);
}

// =========== Private API ============

void scene_redraw(scene *s, gen_frame *new_frames) {
    gen_frame *old_frames = s->frames;
    for (int i = 0; i < s->num_frames; i++) {
        gen_frame old_frame = old_frames[i];
        gen_frame new_frame = new_frames[i];

        int old_height = old_frame.top_height;
        int new_height = new_frame.top_height;
        int delta = new_height - old_height;
        int x = old_frame.x;

        Adafruit_ST7735 *tft = s->tft;

        // Fill or erase pixels from the top boundary depending on the
        // change in height (delta).
        if (delta > 0) {
            tft->fillRect(x, old_height, 1, delta, COL_TER(s));
        } else if (delta < 0) {
            tft->fillRect(x, old_height + delta, 1, -delta, COL_BG(s));
        }

        // Same for the bottom boundary.
        old_height = old_frame.bottom_height;
        new_height = new_frame.bottom_height;
        delta = new_height - old_height;

        int gen_height = s->gen->size.height;
        if (delta > 0) {
            tft->fillRect(x, gen_height - old_height - delta, 1, delta, COL_TER(s));
        } else if (delta < 0) {
            tft->fillRect(x, gen_height - old_height, 1, -delta, COL_BG(s));
        }
    }
}

void scene_initial_draw(scene *s) {
    Adafruit_ST7735 *tft = s->tft;
    tft->fillScreen(COL_BG(s));
    size_t len;
    gen_frame *frames = gen_copy_frames(s->gen, &len);
    for (int i = 0; i < len; i++) {
        gen_frame frame = frames[i];
        tft->fillRect(frame.x, 0, 1, frame.top_height, COL_TER(s));
        tft->fillRect(frame.x, s->gen->size.height - frame.bottom_height, 1, frame.bottom_height, COL_TER(s));
    }
    s->frames = frames;
    s->num_frames = len;
}