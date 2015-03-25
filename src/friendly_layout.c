#include "friendly_layout.h"
#include <pebble.h>

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GBitmap *s_res_image_pbl_100;
static GBitmap *s_res_image_fat_empty_android;
static GFont s_res_bitham_30_black;
static BitmapLayer *s_bitmaplayer_pebble_battery;
static BitmapLayer *s_bitmaplayer_fat_empty_android;
static TextLayer *s_textlayer_1;
static InverterLayer *s_inverterlayer_1;
static TextLayer *s_textlayer_2;

static void initialise_ui(void) {
  s_window = window_create();
  window_set_fullscreen(s_window, true);
  
  s_res_image_pbl_100 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PBL_100);
  s_res_image_fat_empty_android = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_FAT_EMPTY_ANDROID);
  s_res_bitham_30_black = fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK);
  // s_bitmaplayer_pebble_battery
  s_bitmaplayer_pebble_battery = bitmap_layer_create(GRect(93, 10, 50, 20));
  bitmap_layer_set_bitmap(s_bitmaplayer_pebble_battery, s_res_image_pbl_100);
  bitmap_layer_set_background_color(s_bitmaplayer_pebble_battery, GColorWhite);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_bitmaplayer_pebble_battery);
  
  // s_bitmaplayer_fat_empty_android
  s_bitmaplayer_fat_empty_android = bitmap_layer_create(GRect(80, 103, 64, 64));
  bitmap_layer_set_bitmap(s_bitmaplayer_fat_empty_android, s_res_image_fat_empty_android);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_bitmaplayer_fat_empty_android);
  
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
  s_textlayer_2 = text_layer_create(GRect(2, 2, 144, 72));
  text_layer_set_background_color(s_textlayer_2, GColorClear);
  text_layer_set_text(s_textlayer_2, "23:00 asdasdasdasd");
  text_layer_set_font(s_textlayer_2, s_res_bitham_30_black);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_2);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  bitmap_layer_destroy(s_bitmaplayer_pebble_battery);
  bitmap_layer_destroy(s_bitmaplayer_fat_empty_android);
  text_layer_destroy(s_textlayer_1);
  inverter_layer_destroy(s_inverterlayer_1);
  text_layer_destroy(s_textlayer_2);
  gbitmap_destroy(s_res_image_pbl_100);
  gbitmap_destroy(s_res_image_fat_empty_android);
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
