#pragma once
#include <pebble.h>
#include "sidebar_widgets.h"

#define PERSIST_KEY 0

typedef enum {
  NO_VIBE              = 0,
  VIBE_EVERY_HOUR      = 1,
  VIBE_EVERY_HALF_HOUR = 2
} VibeIntervalType;

typedef struct {
  // color settings
  GColor timeColor;
  GColor timeBgColor;
  GColor sidebarColor;
  GColor sidebarTextColor;

  // general settings
  uint8_t languageId;
  bool showLeadingZero;
  uint8_t clockFontId;

  // vibration settings
  bool btVibe;
  VibeIntervalType hourlyVibe;

  // sidebar settings
  SidebarWidgetType widgets[3];
  bool sidebarOnLeft;
  bool useLargeFonts;
  bool activateDisconnectIcon;

  // battery meter widget settings
  bool showBatteryPct;
  bool disableAutobattery;

  // alt tz widget settings
  char altclockName[8];
  int altclockOffset;

  // dynamic settings (calculated based the currently-selected widgets)
  bool updateScreenEverySecond;
  bool enableAutoBatteryWidget;
  bool enableAltTimeZone;

  // TODO: these shouldn't be dynamic
  GColor iconFillColor;
  GColor iconStrokeColor;
} Settings;

// !! all future settings should be added to the bottom of this structure
// !! do not remove fields from this structure, it will lead to unexpected behaviour
typedef struct {
  GColor timeColor;
  GColor timeBgColor;
  GColor sidebarColor;
  GColor sidebarTextColor;

  // general settings
  uint8_t languageId;
  uint8_t showLeadingZero:1;
  uint8_t clockFontId:7;

  // vibration settings
  uint8_t btVibe:1;
  int8_t hourlyVibe:7;

  // sidebar settings
  uint8_t widgets[3];
  uint8_t sidebarOnLeft:1;
  uint8_t useLargeFonts:1;

  // battery meter widget settings
  uint8_t showBatteryPct:1;
  uint8_t disableAutobattery:1;

  // alt tz widget settings
  char altclockName[8];
  int8_t altclockOffset;

  // bluetooth disconnection icon
  int8_t activateDisconnectIcon:1;
} StoredSettings;

extern Settings globalSettings;

void Settings_init();
void Settings_deinit();
void Settings_loadFromStorage();
void Settings_saveToStorage();
void Settings_loadDefaults();
void Settings_updateDynamicSettings();
