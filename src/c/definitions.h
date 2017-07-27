#pragma once
#define BLUETOOTH_STATE_CHANGED_VIBRATION
#define SHOW_LEADING_ZERO

#ifdef PBL_COLOR
  #define TIME_COLOR       GColorOrange
  #define TIME_BG_COLOR    GColorWhite
  #define SIDEBAR_COLOR    GColorWhite
  #define SIDEBAR_BG_COLOR GColorOrange
#else
  #define TIME_COLOR       GColorBlack
  #define TIME_BG_COLOR    GColorWhite
  #define SIDEBAR_COLOR    GColorBlack
  #define SIDEBAR_BG_COLOR GColorWhite
#endif

// Battery Pos 8 - 5
#define BATTERY_HORIZONAL_POS 3
#define BATTERY_VERTICAL_POS  3

// Bt disconnect/connect indicator pos
#define BT_INDICATOR_HORIZONTAL_POS 3
#define BT_INDICATOR_VERTICAL_POS   52

// Day indicator Pos
#define DAY_HORIZONTAL_POS -5
#define DAY_VERTICAL_POS   138
