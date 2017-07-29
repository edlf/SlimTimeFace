#pragma once
// "Configurations" for the watchface.
// These are done here and for every change that you do, you must rebuild the app

#define BLUETOOTH_STATE_CHANGED_VIBRATION
#define SHOW_LEADING_ZERO

#ifdef PBL_COLOR
  #define TIME_COLOR       GColorOrange
  #define TIME_BG_COLOR    GColorWhite
#else
  #define TIME_COLOR       GColorBlack
  #define TIME_BG_COLOR    GColorWhite
#endif

#define HOUR_FONT_SIZE   80
#define MINUTE_FONT_SIZE 70
