#include <pebble.h>
#include <ctype.h>
#include <math.h>
#include "sidebar.h"

extern GRect screen_rect;

// sidebar icons
GDrawCommandImage* disconnectImage;
GDrawCommandImage* batteryImage;
GDrawCommandImage* batteryChargeImage;

// fonts
GFont smSidebarFont;
GFont mdSidebarFont;

// current day string
char currentDayNum[3];

// "private" functions
// layer update callbacks
void updateRectSidebar(Layer *l, GContext* ctx);

Layer* sidebarLayer;

void Sidebar_init(Window* window) {
  // init the sidebar layer
  screen_rect = layer_get_bounds(window_get_root_layer(window));
  GRect bounds = GRect(screen_rect.size.w - ACTION_BAR_WIDTH, 0, ACTION_BAR_WIDTH, screen_rect.size.h);

  // load fonts
  smSidebarFont = fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD);
  mdSidebarFont = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);

  // load the sidebar graphics
  disconnectImage = gdraw_command_image_create_with_resource(RESOURCE_ID_DISCONNECTED);
  batteryImage = gdraw_command_image_create_with_resource(RESOURCE_ID_BATTERY_BG);
  batteryChargeImage = gdraw_command_image_create_with_resource(RESOURCE_ID_BATTERY_CHARGE);

  sidebarLayer = layer_create(bounds);
  layer_add_child(window_get_root_layer(window), sidebarLayer);
  layer_set_update_proc(sidebarLayer, updateRectSidebar);
}

void Sidebar_deinit() {
  layer_destroy(sidebarLayer);
  gdraw_command_image_destroy(disconnectImage);
  gdraw_command_image_destroy(batteryImage);
  gdraw_command_image_destroy(batteryChargeImage);
}

void Sidebar_redraw() {
  layer_set_frame(sidebarLayer, GRect(screen_rect.size.w - ACTION_BAR_WIDTH, 0, ACTION_BAR_WIDTH, screen_rect.size.h));

  // redraw the layer
  layer_mark_dirty(sidebarLayer);
}

void Sidebar_updateTime(struct tm* timeInfo) {
  // Get current day
  strftime(currentDayNum, sizeof(currentDayNum), "%e", timeInfo);
  // remove padding on date num, if needed
  if(currentDayNum[0] == ' ') {
    currentDayNum[0] = currentDayNum[1];
    currentDayNum[1] = '\0';
  }
}

void updateRectSidebar(Layer *l, GContext* ctx) {
  graphics_context_set_fill_color(ctx, SIDEBAR_BG_COLOR);
  graphics_fill_rect(ctx, layer_get_bounds(l), 0, GCornerNone);

  graphics_context_set_text_color(ctx, SIDEBAR_COLOR);

  // Draw the battery indicator
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
  // End of battery indicator

  // Draw the BT indicator
  if(!bluetooth_connection_service_peek()) {
    gdraw_command_image_recolor(disconnectImage, SIDEBAR_BG_COLOR, SIDEBAR_COLOR);
    gdraw_command_image_draw(ctx, disconnectImage, GPoint(BT_INDICATOR_HORIZONTAL_POS, BT_INDICATOR_VERTICAL_POS));
  }
  // End of bt indicator

  // Draw the day indicator
  graphics_context_set_text_color(ctx, SIDEBAR_COLOR);

  graphics_draw_text(ctx,
                     currentDayNum,
                     mdSidebarFont,
                     GRect(DAY_HORIZONTAL_POS, DAY_VERTICAL_POS, 40, 20),
                     GTextOverflowModeFill,
                     GTextAlignmentCenter,
                     NULL);
  // End of day indicator
}
