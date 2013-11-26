// ArduinoCopter
// geometry.h
//
// Created November 19, 2013
//
// Definitions for some basic geometry structures.

#ifndef __geometry_h__
#define __geometry_h__

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

#endif