#include <pebble.h>
#include <math.h>
#include "util.h"
#include "sidebar_widgets.h"

int SidebarWidgets_xOffset;

// sidebar icons
GDrawCommandImage* dateImage;
GDrawCommandImage* disconnectImage;
GDrawCommandImage* batteryImage;
GDrawCommandImage* batteryChargeImage;

// fonts
GFont smSidebarFont;
GFont mdSidebarFont;
GFont lgSidebarFont;
GFont currentSidebarFont;
GFont batteryFont;

// the date and time strings
char currentDayNum[3];
char currentWeekNum[3];
char altClock[8];

// the widgets
SidebarWidget batteryMeterWidget;
int BatteryMeter_getHeight();
void BatteryMeter_draw(GContext* ctx, int yPosition);

SidebarWidget emptyWidget;
int EmptyWidget_getHeight();
void EmptyWidget_draw(GContext* ctx, int yPosition);

SidebarWidget dateWidget;
int DateWidget_getHeight();
void DateWidget_draw(GContext* ctx, int yPosition);

SidebarWidget btDisconnectWidget;
int BTDisconnect_getHeight();
void BTDisconnect_draw(GContext* ctx, int yPosition);

void SidebarWidgets_init() {
  // load fonts
  smSidebarFont = fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD);
  mdSidebarFont = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  lgSidebarFont = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);

  // load the sidebar graphics
  dateImage = gdraw_command_image_create_with_resource(RESOURCE_ID_DATE_BG);
  disconnectImage = gdraw_command_image_create_with_resource(RESOURCE_ID_DISCONNECTED);
  batteryImage = gdraw_command_image_create_with_resource(RESOURCE_ID_BATTERY_BG);
  batteryChargeImage = gdraw_command_image_create_with_resource(RESOURCE_ID_BATTERY_CHARGE);

  // set up widgets' function pointers correctly
  batteryMeterWidget.getHeight = BatteryMeter_getHeight;
  batteryMeterWidget.draw      = BatteryMeter_draw;

  emptyWidget.getHeight = EmptyWidget_getHeight;
  emptyWidget.draw      = EmptyWidget_draw;

  dateWidget.getHeight = DateWidget_getHeight;
  dateWidget.draw      = DateWidget_draw;

  btDisconnectWidget.getHeight = BTDisconnect_getHeight;
  btDisconnectWidget.draw      = BTDisconnect_draw;
}

void SidebarWidgets_deinit() {
  gdraw_command_image_destroy(dateImage);
  gdraw_command_image_destroy(disconnectImage);
  gdraw_command_image_destroy(batteryImage);
  gdraw_command_image_destroy(batteryChargeImage);
}

void SidebarWidgets_updateFonts() {
  currentSidebarFont = mdSidebarFont;
  batteryFont = smSidebarFont;
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

/********** functions for the empty widget **********/
int EmptyWidget_getHeight() {
  return 0;
}

void EmptyWidget_draw(GContext* ctx, int yPosition) {
  return;
}

/********** functions for the battery meter widget **********/
int BatteryMeter_getHeight() {
  return 14;
}

void BatteryMeter_draw(GContext* ctx, int yPosition) {

  BatteryChargeState chargeState = battery_state_service_peek();
  uint8_t battery_percent = (chargeState.charge_percent > 0) ? chargeState.charge_percent : 5;

  graphics_context_set_text_color(ctx, SIDEBAR_BG_COLOR);

  int batteryPositionY = yPosition - 5; // correct for vertical empty space on battery icon

  if (batteryImage) {
    gdraw_command_image_recolor(batteryImage, SIDEBAR_BG_COLOR, SIDEBAR_COLOR);
    gdraw_command_image_draw(ctx, batteryImage, GPoint(3 + SidebarWidgets_xOffset, batteryPositionY));
  }

  if(chargeState.is_charging) {
    if(batteryChargeImage) {
      // the charge "bolt" icon uses inverted colors
      gdraw_command_image_recolor(batteryChargeImage, SIDEBAR_COLOR, SIDEBAR_BG_COLOR);
      gdraw_command_image_draw(ctx, batteryChargeImage, GPoint(3 + SidebarWidgets_xOffset, batteryPositionY));
    }
  } else {

    int width = roundf(18 * battery_percent / 100.0f);

    graphics_context_set_fill_color(ctx, SIDEBAR_COLOR);

    #ifdef PBL_COLOR
      if(battery_percent <= 20) {
        graphics_context_set_fill_color(ctx, GColorRed);
      }
    #endif

    graphics_fill_rect(ctx, GRect(6 + SidebarWidgets_xOffset, 8 + batteryPositionY, width, 8), 0, GCornerNone);
  }
}

/********** current date widget **********/
int DateWidget_getHeight() {
  return 41;
}

void DateWidget_draw(GContext* ctx, int yPosition) {
  graphics_context_set_text_color(ctx, SIDEBAR_BG_COLOR);

  // compensate for extra space that appears on the top of the date widget
  yPosition -= 7;

  // next, draw the date background
  // (an image in normal mode, a rectangle in large font mode)
  if(dateImage) {
    gdraw_command_image_recolor(dateImage, SIDEBAR_BG_COLOR, SIDEBAR_COLOR);
    gdraw_command_image_draw(ctx, dateImage, GPoint(3 + SidebarWidgets_xOffset, yPosition + 23));
  }

  // next, draw the date number
  graphics_context_set_text_color(ctx, SIDEBAR_COLOR);

  int yOffset = 0;
  yOffset = 26;

  graphics_draw_text(ctx,
                     currentDayNum,
                     currentSidebarFont,
                     GRect(-5 + SidebarWidgets_xOffset, yPosition + yOffset, 40, 20),
                     GTextOverflowModeFill,
                     GTextAlignmentCenter,
                     NULL);

  // switch back to normal color for the rest
  graphics_context_set_text_color(ctx, SIDEBAR_BG_COLOR);
}

/***** Bluetooth Disconnection Widget *****/
int BTDisconnect_getHeight() {
  return 22;
}

void BTDisconnect_draw(GContext* ctx, int yPosition) {
  if(!bluetooth_connection_service_peek()) {
    gdraw_command_image_recolor(disconnectImage, SIDEBAR_BG_COLOR, SIDEBAR_COLOR);
    gdraw_command_image_draw(ctx, disconnectImage, GPoint(3 + SidebarWidgets_xOffset, yPosition));
  }
}
