// ArduinoCopter
// geometry.cpp
//
// Created November 29, 2013
//

#include "geometry.h"

int g_rect_maxy(g_rect r) {
	return r.origin.y + r.size.height;
}

int g_rect_maxx(g_rect r) {
	return r.origin.x + r.size.width;
}

boolean g_rect_intersects(g_rect r1, g_rect r2) {
	return (g_rect_maxx(r1) <= r2.origin.x ||
	       g_rect_maxx(r2) <= r1.origin.x ||
	       g_rect_maxy(r1) <= r2.origin.y ||
	       g_rect_maxy(r2) <= r1.origin.y) ? false : true;
}
