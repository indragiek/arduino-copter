// ArduinoCopter
// scene.cpp
//
// Created November 21, 2013
//

#include "scene.h"
#include "drawing_utils.h"

// Clears the screen and draws the initial scene.
//
// @param s Pointer to the `scene` to draw.
//
static void scene_initial_draw(scene *s);

// Does a partial redraw of the scene for a new set of frames. Only
// updates the pixels that are necessary, versus doing a complete redraw.
//
// @param s             Pointer to the `scene` to redraw.
// @param new_frames    The new array of frames to draw.
//
static void scene_redraw_frames(scene *s, gen_frame *new_frames);

// Generate an array of updated frames for a screen update.
//
// @param s Pointer to the `scene` to generate new frames for.
// @return The updated array of frames.
static gen_frame * scene_update_frames(scene *s);

// Update underlying data for block layout. Handles updating the origins
// of on-screen blocks, removing off-screen blocks, and inserting blocks
// at the appropriate distaince intervals.
//
// @param s Pointer to the `scene` for which to update the blocks.
static void scene_update_blocks(scene *s);

// Inserts a new block at the end of the block rects array. The block is 
// positioned to start at the right edge of the display.
//
// @param s Pointer to the `scene` for which to insert a block.
static void scene_insert_block(scene *s);

// Updates the position of the blocks on screen.
//
// @param s Pointer to the `scene` for which to redraw the blocks.
static void scene_redraw_blocks(scene *s);

// Detects whether there was a collision with an obstacle or boundary.
//
// @param s Pointer to the `scene` for which to check collisions.
// @param r The rect of the object that is potentially colliding with an obstacle.
//
// @return Whether the object is colliding with an obstacle or boundary.
static boolean scene_detect_collision(scene *s, g_rect r);

// Updates the coordinates of the copter based on the given direction.
//
// @param s     Pointer to the `scene` for which to update the position.
// @param dir   The direction that the copter is moving in.
static void scene_update_copter(scene *s, copter_direction dir);

// Redraw the copter on screen.
//
// @param s         Pointer to the `scene` for which to redraw the copter.
// @param color     The color to fill the copter with.
static void scene_redraw_copter(scene *s, int color);

// =========== Constants ============

// Spacing between the edges of the terrain and the obstacle blocks.
static const int block_edge_margin = 10;

// Maximum value of gravity.
static const int max_gravity = 5;

// Maximum value of boost.
static const int max_boost = 10;

// Dampening factor for the movement of the copter.
static const float damping_factor = 0.5;

// Pixel size of the copter.
static const g_size copter_size = {3, 3};

// Array of pixels to use for drawing the copter at an assumed
// origin of {0, 0}
static const g_point copter_pixels[] = {{0, 0}, {0, 1}, {0, 2}, {1, 0}, {1, 1}, {1, 2}, {2, 0}, {2, 1}, {2, 2}};

// =========== Macros ============

// Convenience macros for pulling colors out of a passed in `scene` struct.
#define COL_TER(s)      s->colors.terrain
#define COL_BG(s)       s->colors.background
#define COL_BLCK(s)     s->colors.blocks
#define COL_CPTR(s)     s->colors.copter

// =========== Public API ============
// All Public APIs are documented in scene.h

scene * scene_new(Adafruit_GFX *tft,
                  g_size tft_size, 
                  int spacing, 
                  int max_d, 
                  int blk_d, 
                  g_size blk_size,
                  scene_colors colors) {
    // Calculate maximum number of obstacle blocks that could be present on screen
    // at a given time in order to figure out how large to make the block_rects array.
    int max_blk = ceilf((float)tft_size.width / (float)(blk_size.width + blk_d)) * 2;

    scene *s = (scene *)malloc(sizeof(scene));
    s->tft = tft;
    s->colors = colors;
    s->block_rects = (g_rect *)malloc(max_blk * sizeof(g_rect));
    s->num_blocks = 0;
    s->last_block_d = 0;
    s->block_size = blk_size;
    s->max_block_d = blk_d;
    s->copter_pos = (g_point){10, (tft_size.height / 2) - (copter_size.height / 2)};
    s->copter_gravity = 0;
    s->copter_boost = 0;
    s->collided = false;
    s->gen = gen_new(tft_size, spacing, max_d);
    scene_initial_draw(s);
    return s;
}

boolean scene_update(scene *s, copter_direction dir) {
    // Redraw the scene with the updated frames.
    gen_frame *frames = scene_update_frames(s);
    scene_redraw_frames(s, frames);

    // Update the frame array of the scene struct.
    free(s->frames);
    s->frames = frames;

    scene_redraw_blocks(s);
    scene_update_blocks(s);

    scene_redraw_copter(s, COL_BG(s));
    g_point old_pos = s->copter_pos;
    scene_update_copter(s, dir);
    g_point new_pos = s->copter_pos;

    if (new_pos.y != old_pos.y) {
        scene_redraw_copter(s, COL_CPTR(s));
        g_rect copter_rect = (g_rect){s->copter_pos, copter_size};
        s->collided = scene_detect_collision(s, copter_rect);
    }
    return s->collided;
}

void scene_free(scene *s) {
    free(s->frames);
    free(s->block_rects);
    free(s);
}

// =========== Private API ============

static void scene_redraw_frames(scene *s, gen_frame *new_frames) {
    gen_frame *old_frames = s->frames;
    for (int i = 0; i < s->num_frames; i++) {
        gen_frame old_frame = old_frames[i];
        gen_frame new_frame = new_frames[i];

        int old_height = old_frame.top_height;
        int new_height = new_frame.top_height;
        int delta = new_height - old_height;

        Adafruit_GFX *tft = s->tft;

        // Fill or erase pixels from the top boundary depending on the
        // change in height (delta).
        if (delta > 0) {
            draw_rect(s->tft, (g_rect){{i, old_height}, {1, delta}}, COL_TER(s));
        } else if (delta < 0) {
            draw_rect(s->tft, (g_rect){{i, old_height + delta}, {1, -delta}}, COL_BG(s));
        }

        // Same for the bottom boundary.
        old_height = old_frame.bottom_height;
        new_height = new_frame.bottom_height;
        delta = new_height - old_height;

        int gen_height = s->gen->size.height;
        if (delta > 0) {
            draw_rect(s->tft, (g_rect){{i, gen_height - old_height}, {1, delta}}, COL_TER(s));
        } else if (delta < 0) {
            draw_rect(s->tft, (g_rect){{i, gen_height - old_height}, {1, -delta}}, COL_BG(s));
        }
    }
}

static void scene_initial_draw(scene *s) {
    Adafruit_GFX *tft = s->tft;
    tft->fillScreen(COL_BG(s));

    generator *gen = s->gen;
    size_t len = gen->num_frames;
    size_t bytes = len * sizeof(gen_frame);
    gen_frame *frames = (gen_frame *)malloc(bytes);
    memcpy(frames, gen->frames, bytes);

    for (int i = 0; i < len; i++) {
        gen_frame frame = frames[i];
        draw_rect(s->tft, (g_rect){{i, 0}, {1, frame.top_height}}, COL_TER(s));
        draw_rect(s->tft, (g_rect){{i, gen->size.height - frame.bottom_height}, {1, frame.bottom_height}}, COL_TER(s));
    }

    s->frames = frames;
    s->num_frames = len;
}

static gen_frame * scene_update_frames(scene *s) {
    // Pop the leftmost frame from the generator.
    gen_frame new_frame;
    gen_frame popped_frame = gen_pop_frame(s->gen, &new_frame);

    // Create a copy of the original frames array to update.
    gen_frame *old_frames = s->frames;
    size_t len = s->num_frames;
    gen_frame *frames = (gen_frame *)malloc(len * sizeof(gen_frame));

    // Shift all the other frames to the left and replace the last element
    // in the array with the newly created frame.
    for (int i = 1; i < len; i++) {
        frames[i - 1] = old_frames[i];
    }
    frames[len - 1] = new_frame;
    return frames;
}

static void scene_update_blocks(scene *s) {
    size_t len = s->num_blocks;
    g_rect *rects = s->block_rects;
    if (len) {
        int i = 0;
        while (i < len) {
            g_rect r = rects[i];
            r.origin.x--;

            // Remove the block once it has gone off screen and shift
            // the other blocks to the left one position in the array.
            //
            // Otherwise assign the updated rect back to the original
            // position in the array.
            if (r.origin.x <= -(s->block_size.width)) {
                for (int j = i + 1; j < len; j++) {
                    rects[j - 1] = rects[j];
                }
                len--;
                s->num_blocks--;
            } else {
                rects[i] = r;
                i++;
            }
        }
    }
    // If the required sistance has passed, it's time to insert another block.
    if (s->last_block_d >= s->max_block_d) {
        scene_insert_block(s);
        s->last_block_d = 0;
    } else {
        s->last_block_d++; 
    }
}

static void scene_insert_block(scene *s) {
    // Calculate the minimum and maximum constraints for the origin by taking
    // into account the heights of the last frame, frame delta, block size, etc.
    generator *g = s->gen;
    size_t len = g->num_frames;
    gen_frame f = g->frames[len - 1];
    int max_delta = g->max_delta;
    int min_origin = f.top_height + max_delta + block_edge_margin;
    int max_origin = g->size.height - f.bottom_height - max_delta - block_edge_margin - s->block_size.height;
    int origin = random(min_origin, max_origin);

    g_rect rect = (g_rect){{len, origin}, s->block_size};
    s->num_blocks++;
    s->block_rects[s->num_blocks - 1] = rect;
}

static void scene_redraw_blocks(scene *s) {
    size_t len = s->num_blocks;
    for (int i = 0; i < len; i++) {
        g_rect r = s->block_rects[i];
        g_size slice_size = (g_size){1, r.size.height};
        g_rect erase_rect = (g_rect){{r.origin.x + r.size.width - 1, r.origin.y}, slice_size};
        g_rect fill_rect = (g_rect){{r.origin.x - 1, r.origin.y}, slice_size};

        if (erase_rect.origin.x <= s->gen->size.width) {
            draw_rect(s->tft, erase_rect, COL_BG(s));
        }
        if (fill_rect.origin.x >= 0) {
            draw_rect(s->tft, fill_rect, COL_BLCK(s));
        }
    }
}

static boolean scene_detect_collision(scene *s, g_rect r) {
    boolean col = false;
    for (int i = 0; i < s->num_blocks; i++) {
        g_rect blck_r = s->block_rects[i];
        if (g_rect_intersects(r, blck_r)) {
            col = true;
            break;
        }
    }
    if (col == false) {
        col = gen_detect_collision(s->gen, r);
    }
    return col;
}


static void scene_update_copter(scene *s, copter_direction dir) {
    if (dir == copter_up) {
        if (++s->copter_boost > max_boost) {
            s->copter_boost = max_boost;
        }
    } else {
        if (--s->copter_boost < 0) {
            s->copter_boost = 0;
        }
    }
    if (++s->copter_gravity > max_gravity) {
        s->copter_gravity = max_gravity;
    }
    s->copter_pos.y += (s->copter_gravity - s->copter_boost) * damping_factor;
}

static void scene_redraw_copter(scene *s, int color) {
    for (int i = 0; i < sizeof(copter_pixels); i++) {
        g_point p = copter_pixels[i];
        p.x += s->copter_pos.x;
        p.y += s->copter_pos.y;
        draw_pixel(s->tft, p, color);
    }
}
