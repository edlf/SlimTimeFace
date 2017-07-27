#include <pebble.h>
#include "definitions.h"
#include "watch_face.h"
#include "util.h"

// windows and layers
static Window* mainWindow;
static Layer* windowLayer;
GRect screen_rect;

// current bluetooth state
static bool isPhoneConnected;

void update_clock() {
  time_t rawTime;
  struct tm* timeInfo;

  time(&rawTime);
  timeInfo = localtime(&rawTime);

  watch_face_update_time(timeInfo);
}

void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_clock();

  // redraw all screen
  icons_redraw();
  clock_redraw();
}

/* forces everything on screen to be redrawn -- perfect for keeping track of settings! */
void redrawScreen() {
  window_set_background_color(mainWindow, TIME_BG_COLOR);

  // maybe the language changed!
  update_clock();

  // update the sidebar
  icons_redraw();
  clock_redraw();
}

static void main_window_load(Window *window) {
  window_set_background_color(window, TIME_BG_COLOR);
  watch_face_init(window);

  // Make sure the time is displayed from the start
  redrawScreen();
}

static void main_window_unload(Window *window) {
  watch_face_deinit();
}

void bluetoothStateChanged(bool newConnectionState) {
  #ifdef BLUETOOTH_STATE_CHANGED_VIBRATION
  // if the phone was connected but isn't anymore and the user has opted in,
  // trigger a vibration
  if(!quiet_time_is_active() && isPhoneConnected && !newConnectionState) {
    static uint32_t const segments[] = { 200, 100, 100, 100, 500 };
    VibePattern pat = {
      .durations = segments,
      .num_segments = ARRAY_LENGTH(segments),
      };
    vibes_enqueue_custom_pattern(pat);
  }
  #endif

  isPhoneConnected = newConnectionState;

  icons_redraw();
}

// force the sidebar to redraw any time the battery state changes
void batteryStateChanged(BatteryChargeState charge_state) {
  icons_redraw();
}

// fixes for disappearing elements after notifications
// (from http://codecorner.galanter.net/2016/01/08/solved-issue-with-pebble-framebuffer-after-notification-is-dismissed/)
static void app_focus_changing(bool focusing) {
  if (focusing) {
     layer_set_hidden(windowLayer, true);
  }
}

static void app_focus_changed(bool focused) {
  if (focused) {
     layer_set_hidden(windowLayer, false);
     layer_mark_dirty(windowLayer);
  }
}

static void init() {
  setlocale(LC_ALL, "");

  // Create main Window element and assign to pointer
  mainWindow = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(mainWindow, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(mainWindow, true);

  windowLayer = window_get_root_layer(mainWindow);

  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  bool connected = bluetooth_connection_service_peek();
  bluetoothStateChanged(connected);
  bluetooth_connection_service_subscribe(bluetoothStateChanged);

  // register with battery service
  battery_state_service_subscribe(batteryStateChanged);

  // set up focus change handlers
  app_focus_service_subscribe_handlers((AppFocusHandlers){
    .did_focus = app_focus_changed,
    .will_focus = app_focus_changing
  });
}

static void deinit() {
  // Destroy Window
  window_destroy(mainWindow);

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
