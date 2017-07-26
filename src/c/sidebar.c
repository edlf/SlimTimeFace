#include <pebble.h>
#include <ctype.h>
#include <math.h>
#include "sidebar.h"
#include "sidebar_widgets.h"

#define V_PADDING_DEFAULT 8
#define V_PADDING_COMPACT 4

extern GRect screen_rect;

extern SidebarWidget batteryMeterWidget;
extern SidebarWidget dateWidget;
extern SidebarWidget btDisconnectWidget;

// "private" functions
// layer update callbacks
#ifndef PBL_ROUND
  void updateRectSidebar(Layer *l, GContext* ctx);
#else

  void updateRoundSidebarLeft(Layer *l, GContext* ctx);
  void updateRoundSidebarRight(Layer *l, GContext* ctx);

  // shared drawing stuff between all layers
  void drawRoundSidebar(GContext* ctx, GRect bgBounds, SidebarWidget widget, int widgetXOffset);
#endif

Layer* sidebarLayer;

#ifdef PBL_ROUND
  Layer* sidebarLayer2;
#endif

void Sidebar_init(Window* window) {
  // init the sidebar layer
  screen_rect = layer_get_bounds(window_get_root_layer(window));
  GRect bounds;

  #ifdef PBL_ROUND
    GRect bounds2;
    bounds = GRect(0, 0, 40, screen_rect.size.h);
    bounds2 = GRect(screen_rect.size.w - 40, 0, 40, screen_rect.size.h);
  #else
    bounds = GRect(screen_rect.size.w - ACTION_BAR_WIDTH, 0, ACTION_BAR_WIDTH, screen_rect.size.h);
  #endif

  // init the widgets
  SidebarWidgets_init();

  sidebarLayer = layer_create(bounds);
  layer_add_child(window_get_root_layer(window), sidebarLayer);

  #ifdef PBL_ROUND
    layer_set_update_proc(sidebarLayer, updateRoundSidebarLeft);
  #else
    layer_set_update_proc(sidebarLayer, updateRectSidebar);
  #endif

  #ifdef PBL_ROUND
    sidebarLayer2 = layer_create(bounds2);
    layer_add_child(window_get_root_layer(window), sidebarLayer2);
    layer_set_update_proc(sidebarLayer2, updateRoundSidebarRight);
  #endif
}

void Sidebar_deinit() {
  layer_destroy(sidebarLayer);

  #ifdef PBL_ROUND
    layer_destroy(sidebarLayer2);
  #endif

  SidebarWidgets_deinit();
}

void Sidebar_redraw() {
  #ifndef PBL_ROUND
    layer_set_frame(sidebarLayer, GRect(screen_rect.size.w - ACTION_BAR_WIDTH, 0, ACTION_BAR_WIDTH, screen_rect.size.h));
  #endif

  // redraw the layer
  layer_mark_dirty(sidebarLayer);

  #ifdef PBL_ROUND
    layer_mark_dirty(sidebarLayer2);
  #endif
}

void Sidebar_updateTime(struct tm* timeInfo) {
  SidebarWidgets_updateTime(timeInfo);
}

#ifdef PBL_ROUND

void updateRoundSidebarRight(Layer *l, GContext* ctx) {
  GRect bounds = layer_get_bounds(l);
  GRect bgBounds = GRect(bounds.origin.x, bounds.size.h / -2, bounds.size.h * 2, bounds.size.h * 2);

  drawRoundSidebar(ctx, bgBounds, WIDGET_2, 3);
}

void updateRoundSidebarLeft(Layer *l, GContext* ctx) {
  GRect bounds = layer_get_bounds(l);
  GRect bgBounds = GRect(bounds.origin.x - bounds.size.h * 2 + bounds.size.w, bounds.size.h / -2, bounds.size.h * 2, bounds.size.h * 2);

  drawRoundSidebar(ctx, bgBounds, WIDGET_0, 7);
}

void drawRoundSidebar(GContext* ctx, GRect bgBounds, SidebarWidget widget, int widgetXOffset) {
  SidebarWidgets_updateFonts();

  graphics_context_set_fill_color(ctx, SIDEBAR_BG_COLOR);

  graphics_fill_radial(ctx,
                       bgBounds,
                       GOvalScaleModeFillCircle,
                       100,
                       DEG_TO_TRIGANGLE(0),
                       TRIG_MAX_ANGLE);

  SidebarWidgets_xOffset = widgetXOffset;

  // calculate center position of the widget
  int widgetPosition = bgBounds.size.h / 4 - widget.getHeight() / 2;
  widget.draw(ctx, widgetPosition);
}

#else

void updateRectSidebar(Layer *l, GContext* ctx) {
  GRect bounds = layer_get_unobstructed_bounds(l);

  // this ends up being zero on every rectangular platform besides emery
  SidebarWidgets_xOffset = (ACTION_BAR_WIDTH - 30) / 2;

  SidebarWidgets_updateFonts();

  graphics_context_set_fill_color(ctx, SIDEBAR_BG_COLOR);
  graphics_fill_rect(ctx, layer_get_bounds(l), 0, GCornerNone);

  graphics_context_set_text_color(ctx, SIDEBAR_COLOR);

  SidebarWidget displayWidgets[3];

  displayWidgets[0] = WIDGET_0;
  displayWidgets[1] = WIDGET_1;
  displayWidgets[2] = WIDGET_2;

  int compact_mode_threshold = bounds.size.h - V_PADDING_DEFAULT * 2 - 3;
  int v_padding = V_PADDING_DEFAULT;

  int totalHeight = displayWidgets[0].getHeight() + displayWidgets[1].getHeight() + displayWidgets[2].getHeight();

  // now that they have been compacted, check if they fit a second time,
  // if they still don't fit, our only choice is MURDER (of the middle widget)
  totalHeight = displayWidgets[0].getHeight() + displayWidgets[1].getHeight() + displayWidgets[2].getHeight();
  bool hide_middle_widget = (totalHeight > compact_mode_threshold);
  // printf("Compact Mode Enabled. Total Height: %i, Threshold: %i", totalHeight, compact_mode_threshold);

  // still doesn't fit? try compacting the vertical padding
  totalHeight = displayWidgets[0].getHeight() + displayWidgets[2].getHeight();
  if(totalHeight > compact_mode_threshold) {
    v_padding = V_PADDING_COMPACT;
  }

  // calculate the three widget positions
  int topWidgetPos = v_padding;
  int lowerWidgetPos = bounds.size.h - v_padding - displayWidgets[2].getHeight();

  // vertically center the middle widget using MATH
  int middleWidgetPos = ((lowerWidgetPos - displayWidgets[1].getHeight()) + (topWidgetPos + displayWidgets[0].getHeight())) / 2;

  // draw the widgets
  displayWidgets[0].draw(ctx, topWidgetPos);
  if(!hide_middle_widget) {
    displayWidgets[1].draw(ctx, middleWidgetPos);
  }
  displayWidgets[2].draw(ctx, lowerWidgetPos);
}

#endif
