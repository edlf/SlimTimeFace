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

// the widgets
SidebarWidget batteryMeterWidget;
void BatteryMeter_draw(GContext* ctx, int yPosition);

SidebarWidget dateWidget;
void DateWidget_draw(GContext* ctx, int yPosition);

SidebarWidget btDisconnectWidget;
void BTDisconnect_draw(GContext* ctx, int yPosition);

void SidebarWidgets_init() {
  // load fonts
  smSidebarFont = fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD);
  mdSidebarFont = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);

  // load the sidebar graphics
  disconnectImage = gdraw_command_image_create_with_resource(RESOURCE_ID_DISCONNECTED);
  batteryImage = gdraw_command_image_create_with_resource(RESOURCE_ID_BATTERY_BG);
  batteryChargeImage = gdraw_command_image_create_with_resource(RESOURCE_ID_BATTERY_CHARGE);

  // set up widgets' function pointers correctly
  batteryMeterWidget.draw      = BatteryMeter_draw;
  dateWidget.draw      = DateWidget_draw;
  btDisconnectWidget.draw      = BTDisconnect_draw;
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

/********** functions for the battery meter widget **********/
void BatteryMeter_draw(GContext* ctx, int yPosition) {

  BatteryChargeState chargeState = battery_state_service_peek();
  uint8_t battery_percent = (chargeState.charge_percent > 0) ? chargeState.charge_percent : 5;

  graphics_context_set_text_color(ctx, SIDEBAR_BG_COLOR);

  int batteryPositionY = yPosition - 5; // correct for vertical empty space on battery icon

  if (batteryImage) {
    gdraw_command_image_recolor(batteryImage, SIDEBAR_BG_COLOR, SIDEBAR_COLOR);
    gdraw_command_image_draw(ctx, batteryImage, GPoint(3, batteryPositionY));
  }

  if(chargeState.is_charging) {
    if(batteryChargeImage) {
      // the charge "bolt" icon uses inverted colors
      gdraw_command_image_recolor(batteryChargeImage, SIDEBAR_COLOR, SIDEBAR_BG_COLOR);
      gdraw_command_image_draw(ctx, batteryChargeImage, GPoint(3, batteryPositionY));
    }
  } else {

    int width = roundf(18 * battery_percent / 100.0f);

    graphics_context_set_fill_color(ctx, SIDEBAR_COLOR);

    #ifdef PBL_COLOR
      if(battery_percent <= 20) {
        graphics_context_set_fill_color(ctx, GColorRed);
      }
    #endif

    graphics_fill_rect(ctx, GRect(6, 8 + batteryPositionY, width, 8), 0, GCornerNone);
  }
}

/********** current date widget **********/
void DateWidget_draw(GContext* ctx, int yPosition) {
  graphics_context_set_text_color(ctx, SIDEBAR_COLOR);

  graphics_draw_text(ctx,
                     currentDayNum,
                     mdSidebarFont,
                     GRect(-5, yPosition, 40, 20),
                     GTextOverflowModeFill,
                     GTextAlignmentCenter,
                     NULL);
}

/***** Bluetooth Disconnection Widget *****/
void BTDisconnect_draw(GContext* ctx, int yPosition) {
  if(!bluetooth_connection_service_peek()) {
    gdraw_command_image_recolor(disconnectImage, SIDEBAR_BG_COLOR, SIDEBAR_COLOR);
    gdraw_command_image_draw(ctx, disconnectImage, GPoint(3, yPosition));
  }
}
