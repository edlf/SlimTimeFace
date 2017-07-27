#pragma once
#include <math.h>
#include <pebble.h>
#include "util.h"
#include "definitions.h"

// "public" functions
void watch_face_init(Window* window);
void watch_face_deinit();
void clock_redraw();
void icons_redraw();
void watch_face_update_time(struct tm* time_info);
