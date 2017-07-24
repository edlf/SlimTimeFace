#include <pebble.h>
#include "settings.h"

Settings globalSettings;

void Settings_init() {
  Settings_loadFromStorage();
}

void Settings_deinit() {
  Settings_saveToStorage();
}

/*
 * Load the saved color settings, or if they don't exist load defaults
 */
void Settings_loadFromStorage() {
  /* Check if there are any stored settings */
  if (persist_exists(PERSIST_KEY)) {
    StoredSettings storedSettings;
    memset(&storedSettings,0,sizeof(StoredSettings));
    persist_read_data(PERSIST_KEY, &storedSettings, sizeof(StoredSettings));
    globalSettings.timeColor = storedSettings.timeColor;
    globalSettings.timeBgColor = storedSettings.timeBgColor;
    globalSettings.sidebarColor = storedSettings.sidebarColor;
    globalSettings.sidebarTextColor = storedSettings.sidebarTextColor;
    globalSettings.languageId = storedSettings.languageId;
    globalSettings.showLeadingZero = storedSettings.showLeadingZero;
    globalSettings.clockFontId = storedSettings.clockFontId;
    globalSettings.btVibe = storedSettings.btVibe;
    globalSettings.hourlyVibe = storedSettings.hourlyVibe;
    globalSettings.widgets[0] = storedSettings.widgets[0];
    globalSettings.widgets[1] = storedSettings.widgets[1];
    globalSettings.widgets[2] = storedSettings.widgets[2];
    globalSettings.sidebarOnLeft = storedSettings.sidebarOnLeft;
    globalSettings.useLargeFonts = storedSettings.useLargeFonts;
    globalSettings.showBatteryPct = storedSettings.showBatteryPct;
    globalSettings.disableAutobattery = storedSettings.disableAutobattery;
    memcpy(globalSettings.altclockName, storedSettings.altclockName, 8);
    globalSettings.altclockOffset = storedSettings.altclockOffset;
    globalSettings.activateDisconnectIcon = storedSettings.activateDisconnectIcon;
  } else {
    Settings_loadDefaults();
  }

  Settings_updateDynamicSettings();
}

void Settings_saveToStorage() {
  Settings_updateDynamicSettings();

  // save settings to compressed structure and to persistent storage
  StoredSettings storedSettings;
  storedSettings.timeColor = globalSettings.timeColor;
  storedSettings.timeBgColor = globalSettings.timeBgColor;
  storedSettings.sidebarColor = globalSettings.sidebarColor;
  storedSettings.sidebarTextColor = globalSettings.sidebarTextColor;
  storedSettings.languageId = globalSettings.languageId;
  storedSettings.showLeadingZero = globalSettings.showLeadingZero;
  storedSettings.clockFontId = globalSettings.clockFontId;
  storedSettings.btVibe = globalSettings.btVibe;
  storedSettings.hourlyVibe = globalSettings.hourlyVibe;
  storedSettings.widgets[0] = globalSettings.widgets[0];
  storedSettings.widgets[1] = globalSettings.widgets[1];
  storedSettings.widgets[2] = globalSettings.widgets[2];
  storedSettings.sidebarOnLeft = globalSettings.sidebarOnLeft;
  storedSettings.useLargeFonts = globalSettings.useLargeFonts;
  storedSettings.showBatteryPct = globalSettings.showBatteryPct;
  storedSettings.disableAutobattery = globalSettings.disableAutobattery;
  memcpy(storedSettings.altclockName, globalSettings.altclockName, 8);
  storedSettings.altclockOffset = globalSettings.altclockOffset;
  storedSettings.activateDisconnectIcon = globalSettings.activateDisconnectIcon;

  persist_write_data(PERSIST_KEY, &storedSettings, sizeof(StoredSettings));
}

void Settings_loadDefaults() {
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
  globalSettings.widgets[1] = EMPTY;
  globalSettings.widgets[2] = DATE;

  // set default disconnect icon activation
  globalSettings.activateDisconnectIcon = true;

  strncpy(globalSettings.altclockName, "ALT", sizeof(globalSettings.altclockName));

  globalSettings.showBatteryPct = false;
}

void Settings_updateDynamicSettings() {
  globalSettings.updateScreenEverySecond = false;
  globalSettings.enableAutoBatteryWidget = true;
  globalSettings.enableAltTimeZone = false;

  for(int i = 0; i < 3; i++) {
    // if any widget is "seconds", we'll need to update the sidebar every second
    if(globalSettings.widgets[i] == SECONDS) {
      globalSettings.updateScreenEverySecond = true;
    }

    // if any widget is "battery", disable the automatic battery indication
    if(globalSettings.widgets[i] == BATTERY_METER) {
      globalSettings.enableAutoBatteryWidget = false;
    }

    // if any widget is "alt_time_zone", enable the alternative time calculation
    if(globalSettings.widgets[i] == ALT_TIME_ZONE) {
      globalSettings.enableAltTimeZone = true;
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
