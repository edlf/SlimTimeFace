#pragma once
#include <math.h>
#include <pebble.h>
#include "util.h"
#include "definitions.h"

// "public" functions
void watch_face_init(Window*);
void watch_face_deinit();

void hour_redraw(struct tm*);
void minute_redraw(struct tm*);
void battery_icon_redraw();
void bluetooth_icon_redraw();
void day_redraw(struct tm*);
