#include <pebble.h>
#include <pebble-fctx/fctx.h>
#include <pebble-fctx/fpath.h>
#include <pebble-fctx/ffont.h>
#include "clock_area.h"

char time_hours[3];
char time_minutes[3];

Layer* clock_area_layer;
FFont* avenir;
FFont* avenir_bold;

extern GRect screen_rect;

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

// "public" funcitons
void ClockArea_init(Window* window) {
  // record the screen size, since we NEVER GET IT AGAIN
  screen_rect = layer_get_bounds(window_get_root_layer(window));

  GRect bounds;
  bounds = GRect(0, 0, screen_rect.size.w, screen_rect.size.h);

  // init the clock area layer
  clock_area_layer = layer_create(bounds);
  layer_add_child(window_get_root_layer(window), clock_area_layer);
  layer_set_update_proc(clock_area_layer, update_clock_area_layer);

  // allocate fonts
  avenir =      ffont_create_from_resource(RESOURCE_ID_AVENIR_REGULAR_FFONT);
  avenir_bold = ffont_create_from_resource(RESOURCE_ID_AVENIR_BOLD_FFONT);
}

void ClockArea_deinit() {
  layer_destroy(clock_area_layer);

  // dealloc fonts
  ffont_destroy(avenir);
  ffont_destroy(avenir_bold);
}

void ClockArea_redraw() {
  layer_mark_dirty(clock_area_layer);
}

void ClockArea_update_time(struct tm* time_info) {
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
}
