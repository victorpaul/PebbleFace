#include "friendly_layout.h"
#include <pebble.h>

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GBitmap *s_res_image_pbl_100;
static GBitmap *s_res_image_phone_connection_yes;
static BitmapLayer *s_bitmaplayer_pebble_battery;
static BitmapLayer *s_bitmaplayer_pebble_connected;

static void initialise_ui(void) {
  s_window = window_create();
  window_set_fullscreen(s_window, true);
  
  s_res_image_pbl_100 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PBL_100);
  s_res_image_phone_connection_yes = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PHONE_CONNECTION_YES);
  // s_bitmaplayer_pebble_battery
  s_bitmaplayer_pebble_battery = bitmap_layer_create(GRect(93, 1, 50, 20));
  bitmap_layer_set_bitmap(s_bitmaplayer_pebble_battery, s_res_image_pbl_100);
  bitmap_layer_set_background_color(s_bitmaplayer_pebble_battery, GColorWhite);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_bitmaplayer_pebble_battery);
  
  // s_bitmaplayer_pebble_connected
  s_bitmaplayer_pebble_connected = bitmap_layer_create(GRect(1, 1, 32, 32));
  bitmap_layer_set_bitmap(s_bitmaplayer_pebble_connected, s_res_image_phone_connection_yes);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_bitmaplayer_pebble_connected);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  bitmap_layer_destroy(s_bitmaplayer_pebble_battery);
  bitmap_layer_destroy(s_bitmaplayer_pebble_connected);
  gbitmap_destroy(s_res_image_pbl_100);
  gbitmap_destroy(s_res_image_phone_connection_yes);
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
