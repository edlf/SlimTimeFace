#include <pebble.h>
#include <math.h>
#include "util.h"
#include "sidebar_widgets.h"

// sidebar icons
GDrawCommandImage* disconnectImage;
GDrawCommandImage* batteryImage;
GDrawCommandImage* batteryChargeImage;

// fonts
GFont smSidebarFont;
GFont mdSidebarFont;

// the date and time strings
char currentDayNum[3];
char currentWeekNum[3];
char altClock[8];

void SidebarWidgets_init() {
  // load fonts
  smSidebarFont = fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD);
  mdSidebarFont = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);

  // load the sidebar graphics
  disconnectImage = gdraw_command_image_create_with_resource(RESOURCE_ID_DISCONNECTED);
  batteryImage = gdraw_command_image_create_with_resource(RESOURCE_ID_BATTERY_BG);
  batteryChargeImage = gdraw_command_image_create_with_resource(RESOURCE_ID_BATTERY_CHARGE);
}

void SidebarWidgets_deinit() {
  gdraw_command_image_destroy(disconnectImage);
  gdraw_command_image_destroy(batteryImage);
  gdraw_command_image_destroy(batteryChargeImage);
}

void SidebarWidgets_updateTime(struct tm* timeInfo) {
  // set all the date strings
  strftime(currentDayNum, sizeof(currentDayNum), "%e", timeInfo);
  // remove padding on date num, if needed
  if(currentDayNum[0] == ' ') {
    currentDayNum[0] = currentDayNum[1];
    currentDayNum[1] = '\0';
  }

  strftime(currentWeekNum, sizeof(currentWeekNum), "%V", timeInfo);
}

void BatteryMeter_draw(GContext* ctx) {

  BatteryChargeState chargeState = battery_state_service_peek();
  uint8_t battery_percent = (chargeState.charge_percent > 0) ? chargeState.charge_percent : 5;

  graphics_context_set_text_color(ctx, SIDEBAR_BG_COLOR);

  if (batteryImage) {
    gdraw_command_image_recolor(batteryImage, SIDEBAR_BG_COLOR, SIDEBAR_COLOR);
    gdraw_command_image_draw(ctx, batteryImage, GPoint(BATTERY_HORIZONAL_POS, BATTERY_VERTICAL_POS));
  }

  if(chargeState.is_charging) {
    if(batteryChargeImage) {
      // the charge "bolt" icon uses inverted colors
      gdraw_command_image_recolor(batteryChargeImage, SIDEBAR_COLOR, SIDEBAR_BG_COLOR);
      gdraw_command_image_draw(ctx, batteryChargeImage, GPoint(BATTERY_HORIZONAL_POS, BATTERY_VERTICAL_POS));
    }
  } else {

    int width = roundf(18 * battery_percent / 100.0f);

    graphics_context_set_fill_color(ctx, SIDEBAR_COLOR);

    #ifdef PBL_COLOR
      if(battery_percent <= 20) {
        graphics_context_set_fill_color(ctx, GColorRed);
      }
    #endif

    graphics_fill_rect(ctx, GRect(6, 8 + BATTERY_VERTICAL_POS, width, 8), 0, GCornerNone);
  }
}

void DateWidget_draw(GContext* ctx) {
  graphics_context_set_text_color(ctx, SIDEBAR_COLOR);

  graphics_draw_text(ctx,
                     currentDayNum,
                     mdSidebarFont,
                     GRect(DAY_HORIZONTAL_POS, DAY_VERTICAL_POS, 40, 20),
                     GTextOverflowModeFill,
                     GTextAlignmentCenter,
                     NULL);
}

void BTDisconnect_draw(GContext* ctx) {
  if(!bluetooth_connection_service_peek()) {
    gdraw_command_image_recolor(disconnectImage, SIDEBAR_BG_COLOR, SIDEBAR_COLOR);
    gdraw_command_image_draw(ctx, disconnectImage, GPoint(BT_INDICATOR_HORIZONTAL_POS, BT_INDICATOR_VERTICAL_POS));
  }
}
