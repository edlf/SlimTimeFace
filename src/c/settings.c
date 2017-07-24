#include <pebble.h>
#include "settings.h"

Settings globalSettings;

void Settings_updateDynamicSettings() {
  // temp: if the sidebar is black, use inverted colors for icons
  if(gcolor_equal(globalSettings.sidebarColor, GColorBlack)) {
    globalSettings.iconFillColor = GColorBlack;
    globalSettings.iconStrokeColor = globalSettings.sidebarTextColor; // exciting
  } else {
    globalSettings.iconFillColor = GColorWhite;
    globalSettings.iconStrokeColor = GColorBlack;
  }
}

void Settings_init() {
  // load the default colors
  globalSettings.timeBgColor      = GColorWhite;
  globalSettings.sidebarTextColor = GColorWhite;

  #ifdef PBL_COLOR
    globalSettings.timeColor      = GColorOrange;
    globalSettings.sidebarColor   = GColorOrange;
  #else
    globalSettings.timeColor      = GColorBlack;
    globalSettings.sidebarColor   = GColorBlack;
  #endif

  globalSettings.showLeadingZero = true;

  // set the default widgets
  globalSettings.widgets[0] = BATTERY_METER;
  globalSettings.widgets[1] = BLUETOOTH_DISCONNECT;
  globalSettings.widgets[2] = DATE;

  Settings_updateDynamicSettings();
}
