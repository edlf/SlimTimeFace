#include <pebble.h>
#include <pebble-fctx/fctx.h>
#include <pebble-fctx/fpath.h>
#include <pebble-fctx/ffont.h>
#include "watch_face.h"

// Resources
GDrawCommandImage* image_disconnect;
GDrawCommandImage* image_battery;
GDrawCommandImage* image_battery_charge;

GFont gfont_day;
FFont* ffont_avenir;
FFont* ffont_avenir_bold;

// Draw
Layer* layer_clock_area;
Layer* layer_sidebar;
GRect  grect_screen;

// Strings
char string_hours[3];
char string_minutes[3];
char string_day[3];

// Clock area update callback
void update_layer_clock_area(Layer *l, GContext* ctx) {
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
  fctx_set_text_em_height(&fctx, ffont_avenir, font_size);
  fctx_set_text_em_height(&fctx, ffont_avenir, font_size);

  // draw hours
  time_pos.x = INT_TO_FIXED(bounds.size.w / 2 + h_adjust);
  time_pos.y = INT_TO_FIXED(v_padding + v_adjust);
  fctx_set_offset(&fctx, time_pos);
  fctx_draw_string(&fctx, string_hours, ffont_avenir, GTextAlignmentCenter, FTextAnchorTop);

  //draw minutes
  time_pos.y = INT_TO_FIXED(bounds.size.h - v_padding + v_adjust);
  fctx_set_offset(&fctx, time_pos);
  fctx_draw_string(&fctx, string_minutes, ffont_avenir, GTextAlignmentCenter, FTextAnchorBaseline);
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

  if (image_battery) {
    gdraw_command_image_recolor(image_battery, TIME_BG_COLOR, TIME_COLOR);
    gdraw_command_image_draw(ctx, image_battery, GPoint(BATTERY_HORIZONAL_POS, BATTERY_VERTICAL_POS));
  }

  if(chargeState.is_charging) {
    if(image_battery_charge) {
      // the charge "bolt" icon uses inverted colors
      gdraw_command_image_recolor(image_battery_charge, TIME_COLOR, TIME_BG_COLOR);
      gdraw_command_image_draw(ctx, image_battery_charge, GPoint(BATTERY_HORIZONAL_POS, BATTERY_VERTICAL_POS));
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
    gdraw_command_image_recolor(image_disconnect, TIME_BG_COLOR, TIME_COLOR);
    gdraw_command_image_draw(ctx, image_disconnect, GPoint(BT_INDICATOR_HORIZONTAL_POS, BT_INDICATOR_VERTICAL_POS));
  }
  // End of bt indicator

  // Draw the day indicator
  graphics_context_set_text_color(ctx, TIME_COLOR);

  graphics_draw_text(ctx,
                     string_day,
                     gfont_day,
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
  gfont_day = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);

  // allocate fonts
  ffont_avenir      = ffont_create_from_resource(RESOURCE_ID_AVENIR_REGULAR_FFONT);
  ffont_avenir_bold = ffont_create_from_resource(RESOURCE_ID_AVENIR_BOLD_FFONT);

  // load the sidebar graphics
  image_disconnect     = gdraw_command_image_create_with_resource(RESOURCE_ID_DISCONNECTED);
  image_battery        = gdraw_command_image_create_with_resource(RESOURCE_ID_BATTERY_BG);
  image_battery_charge = gdraw_command_image_create_with_resource(RESOURCE_ID_BATTERY_CHARGE);

  GRect bounds;
  bounds = GRect(0, 0, grect_screen.size.w, grect_screen.size.h);

  // init the clock area layer
  layer_clock_area = layer_create(bounds);
  layer_add_child(window_get_root_layer(window), layer_clock_area);
  layer_set_update_proc(layer_clock_area, update_layer_clock_area);

  // init the sidebar layer
  grect_screen = layer_get_bounds(window_get_root_layer(window));
  bounds = GRect(grect_screen.size.w - ACTION_BAR_WIDTH, 0, ACTION_BAR_WIDTH, grect_screen.size.h);

  layer_sidebar = layer_create(bounds);
  layer_add_child(window_get_root_layer(window), layer_sidebar);
  layer_set_update_proc(layer_sidebar, update_sidebar_area_layer);
}

void watch_face_deinit() {
  layer_destroy(layer_clock_area);
  layer_destroy(layer_sidebar);

  // dealloc fonts
  ffont_destroy(ffont_avenir);
  ffont_destroy(ffont_avenir_bold);

  // dealloc icons
  gdraw_command_image_destroy(image_disconnect);
  gdraw_command_image_destroy(image_battery);
  gdraw_command_image_destroy(image_battery_charge);
}

// Redraws the clock layer
void clock_redraw() {
  layer_mark_dirty(layer_clock_area);
}

// Redraws the icons layer
void icons_redraw() {
  layer_set_frame(layer_sidebar, GRect(grect_screen.size.w - ACTION_BAR_WIDTH, 0,
                                      ACTION_BAR_WIDTH, grect_screen.size.h));

  // redraw the layer
  layer_mark_dirty(layer_sidebar);
}

// Updates time strings
void watch_face_update_time(struct tm* time_info) {
  // hours
  if (clock_is_24h_style()) {
    #ifdef SHOW_LEADING_ZERO
    strftime(string_hours, sizeof(string_hours), "%H", time_info);
    #else
    strftime(string_hours, sizeof(string_hours), "%k", time_info);
    #endif
  } else {
    #ifdef SHOW_LEADING_ZERO
    strftime(string_hours, sizeof(string_hours), "%I", time_info);
    #else
    strftime(string_hours, sizeof(string_hours), "%l", time_info);
    #endif
  }

  // minutes
  strftime(string_minutes, sizeof(string_minutes), "%M", time_info);

  // day
  strftime(string_day, sizeof(string_day), "%e", time_info);
  // remove padding on date num, if needed
  if(string_day[0] == ' ') {
    string_day[0] = string_day[1];
    string_day[1] = '\0';
  }
}
