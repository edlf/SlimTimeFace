#include <pebble.h>
#include <ctype.h>
#include <math.h>
#include "sidebar.h"
#include "sidebar_widgets.h"

extern GRect screen_rect;

extern SidebarWidget batteryMeterWidget;
extern SidebarWidget dateWidget;
extern SidebarWidget btDisconnectWidget;

// "private" functions
// layer update callbacks
void updateRectSidebar(Layer *l, GContext* ctx);

Layer* sidebarLayer;

void Sidebar_init(Window* window) {
  // init the sidebar layer
  screen_rect = layer_get_bounds(window_get_root_layer(window));
  GRect bounds = GRect(screen_rect.size.w - ACTION_BAR_WIDTH, 0, ACTION_BAR_WIDTH, screen_rect.size.h);

  // init the widgets
  SidebarWidgets_init();

  sidebarLayer = layer_create(bounds);
  layer_add_child(window_get_root_layer(window), sidebarLayer);
  layer_set_update_proc(sidebarLayer, updateRectSidebar);
}

void Sidebar_deinit() {
  layer_destroy(sidebarLayer);
  SidebarWidgets_deinit();
}

void Sidebar_redraw() {
  layer_set_frame(sidebarLayer, GRect(screen_rect.size.w - ACTION_BAR_WIDTH, 0, ACTION_BAR_WIDTH, screen_rect.size.h));

  // redraw the layer
  layer_mark_dirty(sidebarLayer);
}

void Sidebar_updateTime(struct tm* timeInfo) {
  SidebarWidgets_updateTime(timeInfo);
}

void updateRectSidebar(Layer *l, GContext* ctx) {
  GRect bounds = layer_get_unobstructed_bounds(l);

  // this ends up being zero on every rectangular platform besides emery
  SidebarWidgets_xOffset = (ACTION_BAR_WIDTH - 30) / 2;

  SidebarWidgets_updateFonts();

  graphics_context_set_fill_color(ctx, SIDEBAR_BG_COLOR);
  graphics_fill_rect(ctx, layer_get_bounds(l), 0, GCornerNone);

  graphics_context_set_text_color(ctx, SIDEBAR_COLOR);

  // calculate the three widget positions
  int topWidgetPos = V_PADDING_DEFAULT;
  int lowerWidgetPos = bounds.size.h - V_PADDING_DEFAULT - WIDGET_2.getHeight();

  // vertically center the middle widget using MATH
  int middleWidgetPos = ((lowerWidgetPos - WIDGET_1.getHeight()) + (topWidgetPos + WIDGET_0.getHeight())) / 2;

  // draw the widgets
  WIDGET_0.draw(ctx, topWidgetPos);
  WIDGET_1.draw(ctx, middleWidgetPos);
  WIDGET_2.draw(ctx, lowerWidgetPos);
}
