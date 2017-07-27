#include <pebble.h>
#include <pebble-fctx/fctx.h>
#include <pebble-fctx/fpath.h>
#include <pebble-fctx/ffont.h>
#include "watch_face.h"

// Resources
GDrawCommandImage* disconnectImage;
GDrawCommandImage* batteryImage;
GDrawCommandImage* batteryChargeImage;

GFont day_font;

FFont* avenir;
FFont* avenir_bold;

// Draw
Layer* clock_area_layer;
Layer* sidebarLayer;
GRect grect_screen;

// Strings
char time_hours[3];
char time_minutes[3];
char time_day[3];

// Clock area update callback
void update_clock_area_layer(Layer *l, GContext* ctx) {
  // check layer bounds
  GRect bounds = layer_get_unobstructed_bounds(l);

  // initialize FCTX, the fancy 3rd party drawing library that all the cool kids use
  FContext fctx;

  fctx_init_context(&fctx, ctx);
  fctx_set_color_bias(&fctx, 0);
  fctx_set_fill_color(&fctx, TIME_COLOR);

  // calculate font size
  int font_size = 4 * bounds.size.h / 7;

  // avenir + avenir bold metrics
  int v_padding = bounds.size.h / 16;
  int h_adjust = 0;
  int v_adjust = 0;

  #ifdef PBL_COLOR
    fctx_enable_aa(true);
  #endif

  // for rectangular watches, adjust X position based on sidebar position
  h_adjust -= ACTION_BAR_WIDTH / 2 + 1;

  FPoint time_pos;
  fctx_begin_fill(&fctx);
  fctx_set_text_em_height(&fctx, avenir, font_size);
  fctx_set_text_em_height(&fctx, avenir, font_size);

  // draw hours
  time_pos.x = INT_TO_FIXED(bounds.size.w / 2 + h_adjust);
  time_pos.y = INT_TO_FIXED(v_padding + v_adjust);
  fctx_set_offset(&fctx, time_pos);
  fctx_draw_string(&fctx, time_hours, avenir, GTextAlignmentCenter, FTextAnchorTop);

  //draw minutes
  time_pos.y = INT_TO_FIXED(bounds.size.h - v_padding + v_adjust);
  fctx_set_offset(&fctx, time_pos);
  fctx_draw_string(&fctx, time_minutes, avenir, GTextAlignmentCenter, FTextAnchorBaseline);
  fctx_end_fill(&fctx);

  fctx_deinit_context(&fctx);
}

// Icon area update callback
void update_sidebar_area_layer(Layer *l, GContext* ctx) {
  // draw icons
  // Draw the battery indicator
  BatteryChargeState chargeState = battery_state_service_peek();
  uint8_t battery_percent = (chargeState.charge_percent > 0) ? chargeState.charge_percent : 5;

  graphics_context_set_text_color(ctx, TIME_BG_COLOR);

  if (batteryImage) {
    gdraw_command_image_recolor(batteryImage, TIME_BG_COLOR, TIME_COLOR);
    gdraw_command_image_draw(ctx, batteryImage, GPoint(BATTERY_HORIZONAL_POS, BATTERY_VERTICAL_POS));
  }

  if(chargeState.is_charging) {
    if(batteryChargeImage) {
      // the charge "bolt" icon uses inverted colors
      gdraw_command_image_recolor(batteryChargeImage, TIME_COLOR, TIME_BG_COLOR);
      gdraw_command_image_draw(ctx, batteryChargeImage, GPoint(BATTERY_HORIZONAL_POS, BATTERY_VERTICAL_POS));
    }
  } else {

    int width = roundf(18 * battery_percent / 100.0f);

    graphics_context_set_fill_color(ctx, TIME_COLOR);

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
    gdraw_command_image_recolor(disconnectImage, TIME_BG_COLOR, TIME_COLOR);
    gdraw_command_image_draw(ctx, disconnectImage, GPoint(BT_INDICATOR_HORIZONTAL_POS, BT_INDICATOR_VERTICAL_POS));
  }
  // End of bt indicator

  // Draw the day indicator
  graphics_context_set_text_color(ctx, TIME_COLOR);

  graphics_draw_text(ctx,
                     time_day,
                     day_font,
                     GRect(DAY_HORIZONTAL_POS, DAY_VERTICAL_POS, 40, 20),
                     GTextOverflowModeFill,
                     GTextAlignmentCenter,
                     NULL);
  // End of day indicator
}

// "public" funcitons
void watch_face_init(Window* window) {
  // record the screen size, since we NEVER GET IT AGAIN
  grect_screen = layer_get_bounds(window_get_root_layer(window));

  // load system fonts
  day_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);

  // allocate fonts
  avenir =      ffont_create_from_resource(RESOURCE_ID_AVENIR_REGULAR_FFONT);
  avenir_bold = ffont_create_from_resource(RESOURCE_ID_AVENIR_BOLD_FFONT);

  // load the sidebar graphics
  disconnectImage = gdraw_command_image_create_with_resource(RESOURCE_ID_DISCONNECTED);
  batteryImage = gdraw_command_image_create_with_resource(RESOURCE_ID_BATTERY_BG);
  batteryChargeImage = gdraw_command_image_create_with_resource(RESOURCE_ID_BATTERY_CHARGE);

  GRect bounds;
  bounds = GRect(0, 0, grect_screen.size.w, grect_screen.size.h);

  // init the clock area layer
  clock_area_layer = layer_create(bounds);
  layer_add_child(window_get_root_layer(window), clock_area_layer);
  layer_set_update_proc(clock_area_layer, update_clock_area_layer);

  // init the sidebar layer
  grect_screen = layer_get_bounds(window_get_root_layer(window));
  bounds = GRect(grect_screen.size.w - ACTION_BAR_WIDTH, 0, ACTION_BAR_WIDTH, grect_screen.size.h);

  sidebarLayer = layer_create(bounds);
  layer_add_child(window_get_root_layer(window), sidebarLayer);
  layer_set_update_proc(sidebarLayer, update_sidebar_area_layer);
}

void watch_face_deinit() {
  layer_destroy(clock_area_layer);
  layer_destroy(sidebarLayer);

  // dealloc fonts
  ffont_destroy(avenir);
  ffont_destroy(avenir_bold);

  // dealloc icons
  gdraw_command_image_destroy(disconnectImage);
  gdraw_command_image_destroy(batteryImage);
  gdraw_command_image_destroy(batteryChargeImage);
}

// Redraws the clock layer
void clock_redraw() {
  layer_mark_dirty(clock_area_layer);
}

// Redraws the icons layer
void icons_redraw() {
  layer_set_frame(sidebarLayer, GRect(grect_screen.size.w - ACTION_BAR_WIDTH, 0,
                                      ACTION_BAR_WIDTH, grect_screen.size.h));

  // redraw the layer
  layer_mark_dirty(sidebarLayer);
}

// Updates time strings
void watch_face_update_time(struct tm* time_info) {
  // hours
  if (clock_is_24h_style()) {
    #ifdef SHOW_LEADING_ZERO
    strftime(time_hours, sizeof(time_hours), "%H", time_info);
    #else
    strftime(time_hours, sizeof(time_hours), "%k", time_info);
    #endif
  } else {
    #ifdef SHOW_LEADING_ZERO
    strftime(time_hours, sizeof(time_hours), "%I", time_info);
    #else
    strftime(time_hours, sizeof(time_hours), "%l", time_info);
    #endif
  }

  // minutes
  strftime(time_minutes, sizeof(time_minutes), "%M", time_info);

  // day
  strftime(time_day, sizeof(time_day), "%e", time_info);
  // remove padding on date num, if needed
  if(time_day[0] == ' ') {
    time_day[0] = time_day[1];
    time_day[1] = '\0';
  }
}
