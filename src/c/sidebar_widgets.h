#pragma once
#include <pebble.h>
#include "definitions.h"

void SidebarWidgets_init();
void SidebarWidgets_deinit();
void SidebarWidgets_updateFonts();
void SidebarWidgets_updateTime(struct tm* timeInfo);

// Widget draw methods
void BatteryMeter_draw(GContext* ctx);
void DateWidget_draw(GContext* ctx);
void BTDisconnect_draw(GContext* ctx);
