#pragma once
#include <math.h>
#include <pebble.h>
#include "util.h"
#include "definitions.h"

// "public" functions
void watch_face_init(Window*);
void watch_face_deinit();
void watch_face_update_time(struct tm*);

void clock_redraw();
void icons_redraw();
