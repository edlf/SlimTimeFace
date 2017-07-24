#pragma once
#include <pebble.h>
#include "sidebar_widgets.h"

typedef struct {
  // color settings
  GColor timeColor;
  GColor timeBgColor;
  GColor sidebarColor;
  GColor sidebarTextColor;

  // general settings
  bool showLeadingZero;
  uint8_t clockFontId;

  // vibration settings
  bool btVibe;

  // sidebar settings
  SidebarWidgetType widgets[3];

  // TODO: these shouldn't be dynamic
  GColor iconFillColor;
  GColor iconStrokeColor;
} Settings;

extern Settings globalSettings;

void Settings_init();
