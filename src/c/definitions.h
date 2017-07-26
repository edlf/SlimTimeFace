#pragma once
#define BLUETOOTH_STATE_CHANGED_VIBRATION
#define SHOW_LEADING_ZERO

#ifdef PBL_COLOR
  #define TIME_COLOR GColorOrange
  #define TIME_BG_COLOR GColorWhite
  #define SIDEBAR_COLOR GColorWhite
  #define SIDEBAR_BG_COLOR GColorOrange
#else
  #define TIME_COLOR GColorBlack
  #define TIME_BG_COLOR GColorWhite
  #define SIDEBAR_COLOR GColorWhite
  #define SIDEBAR_BG_COLOR GColorBlack
#endif

#define WIDGET_0 batteryMeterWidget
#define WIDGET_1 btDisconnectWidget
#define WIDGET_2 dateWidget
