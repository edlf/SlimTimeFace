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
Layer* layer_hour_area;
Layer* layer_minute_area;
Layer* layer_battery_icon;
Layer* layer_bluetooth_icon;
Layer* layer_day;
GRect  grect_screen;

// Strings
char string_hours[3];
char string_minutes[3];
char string_day[3];

// Positions
int battery_layer_posx;
int battery_layer_posy;
int battery_layer_posx_end;
int battery_layer_posy_end;

int bluetooth_layer_posx;
int bluetooth_layer_posy;
int bluetooth_layer_posx_end;
int bluetooth_layer_posy_end;

int day_layer_posx;
int day_layer_posy;
int day_layer_posx_end;
int day_layer_posy_end;

int hour_layer_posx;
int hour_layer_posy;
int hour_layer_posx_end;
int hour_layer_posy_end;

int minute_layer_posx;
int minute_layer_posy;
int minute_layer_posx_end;
int minute_layer_posy_end;

// Hour area update callback
void update_layer_hour_area(Layer *l, GContext* ctx) {
  // initialize FCTX, the fancy 3rd party drawing library that all the cool kids use
  FContext fctx;

  fctx_init_context(&fctx, ctx);
  fctx_set_color_bias(&fctx, 0);
  fctx_set_fill_color(&fctx, TIME_COLOR);

  #ifdef PBL_COLOR
    fctx_enable_aa(true);
  #endif

  FPoint hour_pos;
  fctx_begin_fill(&fctx);
  fctx_set_text_em_height(&fctx, ffont_avenir, HOUR_FONT_SIZE);

  //draw minutes
  hour_pos.x = INT_TO_FIXED(65);
  hour_pos.y = INT_TO_FIXED(80);
  fctx_set_offset(&fctx, hour_pos);
  fctx_draw_string(&fctx, string_hours, ffont_avenir, GTextAlignmentCenter, FTextAnchorBaseline);
  fctx_end_fill(&fctx);

  fctx_deinit_context(&fctx);
}

// Minute area update callback
void update_layer_minute_area(Layer *l, GContext* ctx) {
  // initialize FCTX, the fancy 3rd party drawing library that all the cool kids use
  FContext fctx;

  fctx_init_context(&fctx, ctx);
  fctx_set_color_bias(&fctx, 0);
  fctx_set_fill_color(&fctx, TIME_COLOR);

  #ifdef PBL_COLOR
    fctx_enable_aa(true);
  #endif

  FPoint minute_pos;
  fctx_begin_fill(&fctx);
  fctx_set_text_em_height(&fctx, ffont_avenir, MINUTE_FONT_SIZE);

  //draw minutes
  minute_pos.x = INT_TO_FIXED(65);
  minute_pos.y = INT_TO_FIXED(140);
  fctx_set_offset(&fctx, minute_pos);
  fctx_draw_string(&fctx, string_minutes, ffont_avenir, GTextAlignmentCenter, FTextAnchorBaseline);
  fctx_end_fill(&fctx);

  fctx_deinit_context(&fctx);
}

// Update battery icon layer callback
void update_layer_battery(Layer *l, GContext* ctx) {
  BatteryChargeState chargeState = battery_state_service_peek();
  uint8_t battery_percent = (chargeState.charge_percent > 0) ? chargeState.charge_percent : 5;

  graphics_context_set_text_color(ctx, TIME_BG_COLOR);

  if (image_battery) {
    gdraw_command_image_recolor(image_battery, TIME_BG_COLOR, TIME_COLOR);
    gdraw_command_image_draw(ctx, image_battery, GPoint(3, 0));
  }

  if(chargeState.is_charging) {
    if(image_battery_charge) {
      // the charge "bolt" icon uses inverted colors
      gdraw_command_image_recolor(image_battery_charge, TIME_COLOR, TIME_BG_COLOR);
      gdraw_command_image_draw(ctx, image_battery_charge, GPoint(3, 0));
    }
  } else {

    int width = roundf(18 * battery_percent / 100.0f);

    graphics_context_set_fill_color(ctx, TIME_COLOR);

    #ifdef PBL_COLOR
      if(battery_percent <= 20) {
        graphics_context_set_fill_color(ctx, GColorRed);
      }
    #endif

    graphics_fill_rect(ctx, GRect(6, 8, width, 8), 0, GCornerNone);
  }
}

// Update bluetooth icon layer callback
void update_layer_bluetooth(Layer *l, GContext* ctx) {
  //if(!bluetooth_connection_service_peek()) {
    gdraw_command_image_recolor(image_disconnect, TIME_BG_COLOR, TIME_COLOR);
    gdraw_command_image_draw(ctx, image_disconnect, GPoint(3, 0));
  //}
}

// Update day icon layer callback
void update_layer_day(Layer *l, GContext* ctx) {
  graphics_context_set_text_color(ctx, TIME_COLOR);

  graphics_draw_text(ctx,
                     string_day,
                     gfont_day,
                     GRect(0, 0, 40, 20),
                     GTextOverflowModeFill,
                     GTextAlignmentCenter,
                     NULL);
}

void calc_pos(){
  hour_layer_posx = 0;
  hour_layer_posy = 0;
  hour_layer_posx_end = grect_screen.size.w - ACTION_BAR_WIDTH;
  hour_layer_posy_end = hour_layer_posy + 72;

  minute_layer_posx = 0;
  minute_layer_posy = hour_layer_posy_end + 1;
  minute_layer_posx_end = grect_screen.size.w - ACTION_BAR_WIDTH;
  minute_layer_posy_end = grect_screen.size.h;

  battery_layer_posx = grect_screen.size.w - ACTION_BAR_WIDTH;
  battery_layer_posy = 0;
  battery_layer_posx_end = ACTION_BAR_WIDTH;
  battery_layer_posy_end = battery_layer_posy + 22;

  bluetooth_layer_posx = grect_screen.size.w - ACTION_BAR_WIDTH;
  bluetooth_layer_posy = 40;
  bluetooth_layer_posx_end = ACTION_BAR_WIDTH;
  bluetooth_layer_posy_end = bluetooth_layer_posy + 22;

  day_layer_posx = grect_screen.size.w - ACTION_BAR_WIDTH;
  day_layer_posy = 140;
  day_layer_posx_end = ACTION_BAR_WIDTH;
  day_layer_posy_end = day_layer_posy + 22;
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

  calc_pos();

  // init the hour area layer
  layer_hour_area = layer_create(GRect(hour_layer_posx, hour_layer_posy,
                                       hour_layer_posx_end, hour_layer_posy_end));
  layer_add_child(window_get_root_layer(window), layer_hour_area);
  layer_set_update_proc(layer_hour_area, update_layer_hour_area);

  // init the minute area layer
  layer_minute_area = layer_create(GRect(minute_layer_posx, minute_layer_posy,
                                         minute_layer_posx_end, minute_layer_posy_end));
  layer_add_child(window_get_root_layer(window), layer_minute_area);
  layer_set_update_proc(layer_minute_area, update_layer_minute_area);

  // init the battery icon layer
  layer_battery_icon = layer_create(GRect(battery_layer_posx, battery_layer_posy,
                                          battery_layer_posx_end, battery_layer_posy_end));
  layer_add_child(window_get_root_layer(window), layer_battery_icon);
  layer_set_update_proc(layer_battery_icon, update_layer_battery);

  // init the bluetooth icon layer
  layer_bluetooth_icon = layer_create(GRect(bluetooth_layer_posx, bluetooth_layer_posy,
                                            bluetooth_layer_posx_end, bluetooth_layer_posy_end));
  layer_add_child(window_get_root_layer(window), layer_bluetooth_icon);
  layer_set_update_proc(layer_bluetooth_icon, update_layer_bluetooth);

  // init the day layer
  layer_day = layer_create(GRect(day_layer_posx, day_layer_posy,
                                 day_layer_posx_end, day_layer_posy_end));
  layer_add_child(window_get_root_layer(window), layer_day);
  layer_set_update_proc(layer_day, update_layer_day);
}

void watch_face_deinit() {
  layer_destroy(layer_hour_area);
  layer_destroy(layer_minute_area);
  layer_destroy(layer_battery_icon);
  layer_destroy(layer_bluetooth_icon);
  layer_destroy(layer_day);

  // dealloc fonts
  ffont_destroy(ffont_avenir);
  ffont_destroy(ffont_avenir_bold);

  // dealloc icons
  gdraw_command_image_destroy(image_disconnect);
  gdraw_command_image_destroy(image_battery);
  gdraw_command_image_destroy(image_battery_charge);
}

// Redraws the hour layer
void hour_redraw(struct tm* time_info) {
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

  layer_set_frame(layer_hour_area, GRect(hour_layer_posx, hour_layer_posy,
                                      hour_layer_posx_end, hour_layer_posy_end));

  layer_mark_dirty(layer_hour_area);
}

// Redraws the minute layer
void minute_redraw(struct tm* time_info) {
  strftime(string_minutes, sizeof(string_minutes), "%M", time_info);

  layer_set_frame(layer_minute_area, GRect(minute_layer_posx, minute_layer_posy,
                                      minute_layer_posx_end, minute_layer_posy_end));

  layer_mark_dirty(layer_minute_area);
}

// Redraws the battery layer
void battery_icon_redraw() {
  layer_set_frame(layer_battery_icon, GRect(battery_layer_posx, battery_layer_posy,
                                      battery_layer_posx_end, battery_layer_posy_end));

  // redraw the layer
  layer_mark_dirty(layer_battery_icon);
}

// Redraws the bluetooth layer
void bluetooth_icon_redraw() {
  layer_set_frame(layer_bluetooth_icon, GRect(bluetooth_layer_posx, bluetooth_layer_posy,
                                      bluetooth_layer_posx_end, bluetooth_layer_posy_end));

  // redraw the layer
  layer_mark_dirty(layer_bluetooth_icon);
}

// Redraws the day layer
void day_redraw(struct tm* time_info) {
  strftime(string_day, sizeof(string_day), "%e", time_info);
  // remove padding on date num, if needed
  if(string_day[0] == ' ') {
    string_day[0] = string_day[1];
    string_day[1] = '\0';
  }

  layer_set_frame(layer_day, GRect(day_layer_posx, day_layer_posy,
                                      day_layer_posx_end, day_layer_posy_end));

  // redraw the layer
  layer_mark_dirty(layer_day);
}
