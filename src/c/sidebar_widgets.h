#pragma once
#include <pebble.h>
#include "definitions.h"

/*
 * "Compact Mode" is a global setting shared by all widgets, which determines
 * whether they should try to reduce their padding. Intended to allow larger
 * widgets to fit when vertical screen space is lacking
 */
extern bool SidebarWidgets_useCompactMode;

/*
 * A global x offset used for nudging the widgets left and right
 * Included for round support
 */
extern int SidebarWidgets_xOffset;

typedef struct {
  /*
   * Returns the pixel height of the widget, taking into account all current
   * settings that would affect this, such as font size
   */
  int (*getHeight)();

  /*
   * Draws the widget using the provided graphics context
   */
  void (*draw)(GContext* ctx, int yPosition);
} SidebarWidget;

void SidebarWidgets_init();
void SidebarWidgets_deinit();
void SidebarWidgets_updateFonts();
void SidebarWidgets_updateTime(struct tm* timeInfo);
