#include "friendly_layout.h"
#include <pebble.h>

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GFont s_res_gothic_28_bold;
static TextLayer *s_textlayer_1;
static InverterLayer *s_inverterlayer_1;
static TextLayer *s_textlayer_2;
static TextLayer *s_watch_battery_text;

static void initialise_ui(void) {
  s_window = window_create();
  window_set_fullscreen(s_window, true);
  
  s_res_gothic_28_bold = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
  // s_textlayer_1
  s_textlayer_1 = text_layer_create(GRect(94, 127, 36, 32));
  text_layer_set_background_color(s_textlayer_1, GColorClear);
  text_layer_set_text(s_textlayer_1, "100% (usb)");
  text_layer_set_text_alignment(s_textlayer_1, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_1);
  
  // s_inverterlayer_1
  s_inverterlayer_1 = inverter_layer_create(GRect(94, 127, 36, 30));
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_inverterlayer_1);
  
  // s_textlayer_2
  s_textlayer_2 = text_layer_create(GRect(1, 36, 144, 72));
  text_layer_set_background_color(s_textlayer_2, GColorClear);
  text_layer_set_text(s_textlayer_2, "23:00 April 17");
  text_layer_set_text_alignment(s_textlayer_2, GTextAlignmentCenter);
  text_layer_set_font(s_textlayer_2, s_res_gothic_28_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_2);
  
  // s_watch_battery_text
  s_watch_battery_text = text_layer_create(GRect(0, 0, 144, 15));
  text_layer_set_background_color(s_watch_battery_text, GColorBlack);
  text_layer_set_text_color(s_watch_battery_text, GColorWhite);
  text_layer_set_text(s_watch_battery_text, "watch battery");
  text_layer_set_text_alignment(s_watch_battery_text, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_watch_battery_text);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  text_layer_destroy(s_textlayer_1);
  inverter_layer_destroy(s_inverterlayer_1);
  text_layer_destroy(s_textlayer_2);
  text_layer_destroy(s_watch_battery_text);
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_friendly_layout(void) {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_friendly_layout(void) {
  window_stack_remove(s_window, true);
}
