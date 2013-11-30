// ArduinoCopter
// geometry.h
//
// Created November 19, 2013
//
// Definitions for some basic geometry structures.

#ifndef __geometry_h__
#define __geometry_h__
#include <Arduino.h>

typedef struct {
    int x;
    int y;
} g_point;

typedef struct {
    int width;
    int height;
} g_size;

typedef struct {
    g_point origin;
    g_size size;
} g_rect;

// Calculates the maximum Y-coordinate of the given rectangle
// (r.origin.y + r.size.height)
//
// @param r The rect for which to calculate the max Y.
// @return The max Y value.
int g_rect_maxy(g_rect r);

// Calculates the maxium X-coordinate of the given rectangle
// (r.origin.x + r.size.width)
//
// @param r The rect for which to calculate the max X.
// @return The max X value.
int g_rect_maxx(g_rect r);

// Determines whether two rectangles intersect each other.
//
// @param r1 The first rectangle.
// @param r2 The second rectangle.
// @return Whether r1 and r2 intersect.
boolean g_rect_intersects(g_rect r1, g_rect r2);

#endif
