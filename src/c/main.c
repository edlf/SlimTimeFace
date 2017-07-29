#include <pebble.h>
#include "definitions.h"
#include "watch_face.h"

// windows and layers
static Window* window_main;
static Layer* layer_window;

// current bluetooth state
static bool bt_connected;

void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  switch (units_changed) {
    // just in case
    default:
    case DAY_UNIT:
      day_redraw(tick_time);
      hour_redraw(tick_time);
      minute_redraw(tick_time);
      break;

    case HOUR_UNIT:
      hour_redraw(tick_time);
      minute_redraw(tick_time);
      break;

    case MINUTE_UNIT:
      minute_redraw(tick_time);
      break;

    case SECOND_UNIT:
      break;
  }
}

// forces everything on screen to be redrawn
void redraw_screen() {
  window_set_background_color(window_main, TIME_BG_COLOR);

  // get current time since this is not a tick event
  time_t rawTime;
  struct tm* timeInfo;

  time(&rawTime);
  timeInfo = localtime(&rawTime);

  // redraw the clock
  hour_redraw(timeInfo);
  minute_redraw(timeInfo);

  // redraw the icons
  battery_icon_redraw();
  bluetooth_icon_redraw();
  day_redraw(timeInfo);
}

static void main_window_load(Window *window) {
  window_set_background_color(window, TIME_BG_COLOR);
  watch_face_init(window);

  // Make sure the time is displayed from the start
  redraw_screen();
}

static void main_window_unload(Window *window) {
  watch_face_deinit();
}

// force bt icon redraw everytime the bt connection status changes
void bluetooth_state_changed_callback(bool newConnectionState) {
  #ifdef BLUETOOTH_STATE_CHANGED_VIBRATION
  // if the phone was connected but isn't anymore and the user has opted in,
  // trigger a vibration
  if(!quiet_time_is_active() && bt_connected && !newConnectionState) {
    static uint32_t const segments[] = { 200, 100, 100, 100, 500 };
    VibePattern pat = {
      .durations = segments,
      .num_segments = ARRAY_LENGTH(segments),
      };
    vibes_enqueue_custom_pattern(pat);
  }
  #endif

  bt_connected = newConnectionState;

  bluetooth_icon_redraw();
}

// force the sidebar to redraw any time the battery state changes
void battery_state_changed_callback(BatteryChargeState charge_state) {
  battery_icon_redraw();
}

// fixes for disappearing elements after notifications
// (from http://codecorner.galanter.net/2016/01/08/solved-issue-with-pebble-framebuffer-after-notification-is-dismissed/)
static void app_focus_changing(bool focusing) {
  if (focusing) {
     layer_set_hidden(layer_window, true);
  }
}

static void app_focus_changed(bool focused) {
  if (focused) {
     layer_set_hidden(layer_window, false);
     layer_mark_dirty(layer_window);
  }
}

static void init() {
  setlocale(LC_ALL, "");

  // Create main Window element and assign to pointer
  window_main = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(window_main, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(window_main, true);

  layer_window = window_get_root_layer(window_main);

  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  bool connected = bluetooth_connection_service_peek();
  bluetooth_state_changed_callback(connected);
  bluetooth_connection_service_subscribe(bluetooth_state_changed_callback);

  // register with battery service
  battery_state_service_subscribe(battery_state_changed_callback);

  // set up focus change handlers
  app_focus_service_subscribe_handlers((AppFocusHandlers){
    .did_focus = app_focus_changed,
    .will_focus = app_focus_changing
  });
}

static void deinit() {
  // Destroy Window
  window_destroy(window_main);

  tick_timer_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
  battery_state_service_unsubscribe();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
  return 0;
}
