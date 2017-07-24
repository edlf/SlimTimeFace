#include <pebble.h>
#include "settings.h"

Settings globalSettings;

void Settings_updateDynamicSettings() {
  globalSettings.enableAutoBatteryWidget = true;

  for(int i = 0; i < 3; i++) {
    // if any widget is "battery", disable the automatic battery indication
    if(globalSettings.widgets[i] == BATTERY_METER) {
      globalSettings.enableAutoBatteryWidget = false;
    }
  }

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
  globalSettings.timeBgColor      = GColorBlack;
  globalSettings.sidebarTextColor = GColorBlack;

  #ifdef PBL_COLOR
    globalSettings.timeColor      = GColorOrange;
    globalSettings.sidebarColor   = GColorOrange;
  #else
    globalSettings.timeColor      = GColorWhite;
    globalSettings.sidebarColor   = GColorLightGray;
  #endif

  // set the default widgets
  globalSettings.widgets[0] = BATTERY_METER;
  globalSettings.widgets[1] = BLUETOOTH_DISCONNECT;
  globalSettings.widgets[2] = DATE;

  // set default disconnect icon activation
  globalSettings.activateDisconnectIcon = true;
  Settings_updateDynamicSettings();
}
