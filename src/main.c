#include <pebble.h> //screen resolution 144x168
#include <common.h>

static Window *s_main_window;
static TextLayer *s_phone_battery_layer;
static TextLayer *s_weather_layer;

// global variable
static int batteryLevel = KEY_BATTERY_LEVEL_MIN;
static int batteryStatus = KEY_BATTERY_CHARGING_NONE;
static bool phoneConnected = false;
static int networkStatus = KEY_NETWORK_OFF;

static GBitmap *s_res_image_pbl;
static GBitmap *s_res_image_phone_network;

static GBitmap *s_res_image_fat_empty_android;
static BitmapLayer *s_bitmaplayer_fat_empty_android;
static InverterLayer *s_inverterlayer_phone_battery_level;

static BitmapLayer *s_bitmaplayer_pebble_battery;
static BitmapLayer *s_bitmaplayer_network;
static TextLayer *s_textlayer_time;

// Pebble battery
static void pebbleBatteryHandler(BatteryChargeState charge_state) {
  s_res_image_pbl = gbitmap_create_with_resource(getPebbleBatteryImageResource(charge_state));
  bitmap_layer_set_bitmap(s_bitmaplayer_pebble_battery, s_res_image_pbl);
}

// update time
static void updateTime(){
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  static char formattedTime[32];
  if(clock_is_24h_style() == true) {
    strftime(formattedTime, sizeof(formattedTime), "%H:%M%n%B %d", tick_time);
  } else {
    strftime(formattedTime, sizeof(formattedTime), "%I:%M%n%B %d", tick_time);
  }
  // %A(%u)%n %d %B(%m)
  text_layer_set_text(s_textlayer_time,formattedTime);
}

// calls update time
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  updateTime();
}

void bluetoothHandler(bool connected) {
  phoneConnected = connected;
  pushPhoneBatteryToLayout(s_inverterlayer_phone_battery_level,s_phone_battery_layer,batteryLevel,batteryStatus,phoneConnected);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  Tuple *t = dict_read_first(iterator);

  // Process all pairs present
  while (t != NULL) {
    switch (t->key){
      case KEY_BATTERY_CHARGING_NONE:
      case KEY_BATTERY_CHARGING_USB:
      case KEY_BATTERY_CHARGING_SET:
      case KEY_BATTERY_CHARGING_WIRELESS:
        batteryStatus = t->key;
        persist_write_int(KEY_BATTERY_CHARGING,batteryStatus);
        pushPhoneBatteryToLayout(s_inverterlayer_phone_battery_level,s_phone_battery_layer,batteryLevel,batteryStatus,phoneConnected);
        break;
      case KEY_BATTERY_LEVEL_MIN ... KEY_BATTERY_LEVEL_MAX:
        batteryLevel = t->key;
        persist_write_int(KEY_BATTERY_LEVEL,batteryLevel);      
        pushPhoneBatteryToLayout(s_inverterlayer_phone_battery_level,s_phone_battery_layer,batteryLevel,batteryStatus,phoneConnected);
        break;
      case KEY_NETWORK_OFF:
      case KEY_NETWORK_WIFI:
      //case KEY_NETWORK_WIFI_MOBILE:
      case KEY_NETWORK_MOBILE:
        networkStatus = t->key;
        persist_write_int(KEY_NETWORK,networkStatus);
        pushPhoneNetworkStatusToLayout(s_bitmaplayer_network,s_res_image_phone_network,networkStatus);
        break;
      case KEY_WEATHER:
        text_layer_set_text(s_weather_layer,t->value->cstring);
        persist_write_string(KEY_WEATHER, t->value->cstring);
        break;
    }
    
    t = dict_read_next(iterator);
  }
}

static void main_window_load(Window *window) {  
  Layer *window_layer = window_get_root_layer(window);
  window_set_fullscreen(window, true);

  // READ FROM DB
  batteryLevel = persist_read_int(KEY_BATTERY_LEVEL);
  batteryStatus = persist_read_int(KEY_BATTERY_CHARGING);
  networkStatus = persist_read_int(KEY_NETWORK);

  // CREATE WEATHER LAYER
  s_weather_layer = text_layer_create(GRect(0, 72, PEBBLE_SCREEN_WIDTH,40)); 
  text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_weather_layer));

  // CREATE LAYER pebble battery
  s_bitmaplayer_pebble_battery = bitmap_layer_create(GRect(94, 10, 50, 20));
  layer_add_child(window_layer, (Layer *)s_bitmaplayer_pebble_battery);
  // CREATE NETWORK LAYOUT
  s_bitmaplayer_network = bitmap_layer_create(GRect(1, 103, 64, 64));
  layer_add_child(window_layer, (Layer *)s_bitmaplayer_network);
  // CREATE TIME LAYOUT
  s_res_image_fat_empty_android = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_FAT_EMPTY_ANDROID);
  s_textlayer_time = text_layer_create(GRect(0, 0, 144, 72));
  text_layer_set_font(s_textlayer_time, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
  text_layer_set_background_color(s_textlayer_time, GColorClear);
  layer_add_child(window_layer, (Layer *)s_textlayer_time);  
  // CREATE PHONE BATTERY LAYOUT IMAGE
  s_bitmaplayer_fat_empty_android = bitmap_layer_create(GRect(80, 103, 64, 64));
  bitmap_layer_set_bitmap(s_bitmaplayer_fat_empty_android, s_res_image_fat_empty_android);
  layer_add_child(window_layer, (Layer *)s_bitmaplayer_fat_empty_android);
  // CREATE PHONE BATTERY LAYOUT TEXT
  s_phone_battery_layer = text_layer_create(GRect(TRANSPARENT_BATTERY_X,TRANSPARENT_BATTERY_Y,TRANSPARENT_BATTERY_W,TRANSPARENT_BATTERY_H));
  text_layer_set_text_alignment(s_phone_battery_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, (Layer *)s_phone_battery_layer);
  // CREATE PHONE BATTERY GRAPHIC LEVEL LAYOUT
  s_inverterlayer_phone_battery_level = inverter_layer_create(GRect(TRANSPARENT_BATTERY_X,TRANSPARENT_BATTERY_Y,TRANSPARENT_BATTERY_W,TRANSPARENT_BATTERY_H));
  layer_add_child(window_layer, (Layer *)s_inverterlayer_phone_battery_level);
  
  //PEBBLE EVENTS
  bluetooth_connection_service_subscribe(bluetoothHandler);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  battery_state_service_subscribe(pebbleBatteryHandler);
  
  // UPDATE PEBBLE STATUS
  bluetoothHandler(bluetooth_connection_service_peek());
  pebbleBatteryHandler(battery_state_service_peek());
  updateTime();
  
  // UPDATE PHONE STATUS
  pushPhoneBatteryToLayout(s_inverterlayer_phone_battery_level,s_phone_battery_layer,batteryLevel,batteryStatus,phoneConnected);
  pushPhoneNetworkStatusToLayout(s_bitmaplayer_network,s_res_image_phone_network,networkStatus);
  
  static char weather[16];
  persist_read_string(KEY_WEATHER,weather,sizeof(weather));
  text_layer_set_text(s_weather_layer,weather);
}

static void main_window_unload(Window *window) {
  // Destroy output TextLayer
  text_layer_destroy(s_phone_battery_layer);
  text_layer_destroy(s_weather_layer);
  text_layer_destroy(s_textlayer_time);
  
  bitmap_layer_destroy(s_bitmaplayer_pebble_battery);
  bitmap_layer_destroy(s_bitmaplayer_network);
  
  gbitmap_destroy(s_res_image_pbl);
  gbitmap_destroy(s_res_image_phone_network);  
  gbitmap_destroy(s_res_image_fat_empty_android);
  
  inverter_layer_destroy(s_inverterlayer_phone_battery_level);  
}

static void init() {
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);

  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  // Create main Window
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_stack_push(s_main_window, true);
}

static void deinit() {
  // Destroy main Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}