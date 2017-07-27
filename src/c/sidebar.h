#pragma once
#include <pebble.h>
#include "util.h"
#include "definitions.h"

// "public" functions
void Sidebar_init(Window* window);
void Sidebar_deinit();
void Sidebar_redraw();
void Sidebar_updateTime(struct tm* timeInfo);
