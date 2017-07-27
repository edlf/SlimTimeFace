#pragma once
#include <pebble.h>
#include "definitions.h"

typedef struct {
  /*
   * Draws the widget using the provided graphics context
   */
  void (*draw)(GContext* ctx, int yPosition);
} SidebarWidget;

void SidebarWidgets_init();
void SidebarWidgets_deinit();
void SidebarWidgets_updateFonts();
void SidebarWidgets_updateTime(struct tm* timeInfo);
