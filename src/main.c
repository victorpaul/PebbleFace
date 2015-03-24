#include <pebble.h> //screen resolution 144x168
#include <common.h>

static Window *s_main_window;
static TextLayer *s_phone_battery_layer;
static TextLayer *s_weather_layer;

// global variable
static int batteryLevel = KEY_BATTERY_LEVEL_MIN;
static int batteryStatus = KEY_BATTERY_CHARGING_NONE;
static int networkStatus = KEY_NETWORK_OFF;

static GBitmap *s_res_image_pbl;
static GBitmap *s_res_image_phone_connection;
static GBitmap *s_res_image_phone_network;
static BitmapLayer *s_bitmaplayer_pebble_battery;
static BitmapLayer *s_bitmaplayer_pebble_connection;
static BitmapLayer *s_bitmaplayer_network;

static TextLayer *s_textlayer_time;

static void pebbleBatteryHandler(BatteryChargeState charge_state) {
  s_res_image_pbl = gbitmap_create_with_resource(getPebbleBatteryImageResource(charge_state));
  bitmap_layer_set_bitmap(s_bitmaplayer_pebble_battery, s_res_image_pbl);
}

static void updateTime(){
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  static char formattedTime[16];
  if(clock_is_24h_style() == true) {
    strftime(formattedTime, sizeof(formattedTime), "%H:%M", tick_time);
  } else {
    strftime(formattedTime, sizeof(formattedTime), "%I:%M", tick_time);
  }
  // %A(%u)%n %d %B(%m)
  text_layer_set_text(s_textlayer_time,formattedTime);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  updateTime();
}

void bluetoothHandler(bool connected) {
  if (connected) {
    s_res_image_phone_connection = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PHONE_CONNECTION_YES);
  } else {
    s_res_image_phone_connection = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PHONE_CONNECTION_NO);
  }
  bitmap_layer_set_bitmap(s_bitmaplayer_pebble_connection, s_res_image_phone_connection);
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
        pushPhoneBatteryToLayout(s_phone_battery_layer,batteryLevel,batteryStatus);
        break;
      case KEY_BATTERY_LEVEL_MIN ... KEY_BATTERY_LEVEL_MAX:
        batteryLevel = t->key;
        persist_write_int(KEY_BATTERY_LEVEL,batteryLevel);
        pushPhoneBatteryToLayout(s_phone_battery_layer,batteryLevel,batteryStatus);
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

  int lineHeight = PEBBLE_SCREEN_HEIGHT/4;
  int level1 = 0;
  int level2 = lineHeight;
  int level3 = lineHeight * 2;
  int level4 = lineHeight * 3;

  s_phone_battery_layer = text_layer_create(GRect(PEBBLE_SCREEN_WIDTH/2, level3, PEBBLE_SCREEN_WIDTH/2, lineHeight));
  s_weather_layer = text_layer_create(GRect(0, level2, PEBBLE_SCREEN_WIDTH,lineHeight));

  applyTextStyle(s_phone_battery_layer);
  applyTextStyle(s_weather_layer);
  
  s_bitmaplayer_pebble_battery = bitmap_layer_create(GRect(94, 1, 50, 20));
  s_bitmaplayer_pebble_connection = bitmap_layer_create(GRect(1, 1, 32, 32));
  s_bitmaplayer_network = bitmap_layer_create(GRect(1, 104, 64, 64));
    
  // s_textlayer_time
  s_textlayer_time = text_layer_create(GRect(35, 0, 50, 28));
  text_layer_set_font(s_textlayer_time, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  
  layer_add_child(window_layer, (Layer *)s_textlayer_time);
  layer_add_child(window_layer, (Layer *)s_bitmaplayer_pebble_connection);
  layer_add_child(window_layer, (Layer *)s_bitmaplayer_pebble_battery);
  layer_add_child(window_layer, (Layer *)s_bitmaplayer_network);
  
  layer_add_child(window_layer, text_layer_get_layer(s_phone_battery_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_weather_layer));

  //handle btconnection
  bluetooth_connection_service_subscribe(bluetoothHandler);
  bluetoothHandler(bluetooth_connection_service_peek());

  // handle time
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  updateTime();

  // handle pebble batterry
  battery_state_service_subscribe(pebbleBatteryHandler);
  pebbleBatteryHandler(battery_state_service_peek());
  
  batteryStatus = persist_read_int(KEY_BATTERY_CHARGING);
  batteryLevel = persist_read_int(KEY_BATTERY_LEVEL);
  pushPhoneBatteryToLayout(s_phone_battery_layer,batteryLevel,batteryStatus);
  
  networkStatus = persist_read_int(KEY_NETWORK);
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
  bitmap_layer_destroy(s_bitmaplayer_pebble_connection);
  bitmap_layer_destroy(s_bitmaplayer_network);
  
  gbitmap_destroy(s_res_image_pbl);
  gbitmap_destroy(s_res_image_phone_connection);
  gbitmap_destroy(s_res_image_phone_network);  
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