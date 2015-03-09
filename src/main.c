#include <pebble.h> //screen resolution 144x168
#include <common.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_battery_layer;
static TextLayer *s_phone_battery_layer;
static TextLayer *s_date_layer;
static TextLayer *s_bt_layer;
static TextLayer *s_network_layer;
static TextLayer *s_weather_layer;

// global variable
static int batteryLevel = KEY_BATTERY_LEVEL_MIN;
static int batteryStatus = KEY_BATTERY_CHARGING_NONE;
static int networkStatus = KEY_NETWORK_OFF;

static void battery_handler(BatteryChargeState charge_state) {
  pushBatteryStateToLayout(charge_state,s_battery_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  pushTimeToLayer(s_time_layer);
}

void bluetoothHandler(bool connected) {
  if (connected) {
    text_layer_set_text(s_bt_layer, "Z");
  } else {
    text_layer_set_text(s_bt_layer, "-");
  }  
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  Tuple *t = dict_read_first(iterator);

  // Process all pairs present
  while (t != NULL) {
    switch (t->key){
      case KEY_BATTERY_CHARGING_NONE:
      case KEY_BATTERY_CHARGING_USB:
      case KEY_BATTERY_CHARGING_SET:
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
      case KEY_NETWORK_WIFI_MOBILE:
      case KEY_NETWORK_MOBILE:
        networkStatus = t->key;
        persist_write_int(KEY_NETWORK,networkStatus);
        pushPhoneNetworkStatusToLayout(s_network_layer,networkStatus);
        break;
      case KEY_WEATHER:
        text_layer_set_text(s_weather_layer,t->value->cstring);
        persist_write_string(KEY_WEATHER, t->value->cstring);
        break;
      case KEY_DATE:
        text_layer_set_text(s_date_layer,t->value->cstring);
        persist_write_string(KEY_DATE, t->value->cstring);
        break;
    }
    
    t = dict_read_next(iterator);
  }
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);

  int lineHeight = PEBBLE_SCREEN_HEIGHT/4;
  int level1 = 0;
  int level2 = lineHeight;
  int level3 = lineHeight * 2;
  int level4 = lineHeight * 3;

  s_time_layer = text_layer_create(GRect(0, level1, PEBBLE_SCREEN_WIDTH/2, lineHeight));
  s_battery_layer = text_layer_create(GRect(PEBBLE_SCREEN_WIDTH/2, level1, PEBBLE_SCREEN_WIDTH/2, lineHeight));
  s_bt_layer = text_layer_create(GRect(0, level2, 20,lineHeight));
  s_date_layer = text_layer_create(GRect(20, level2, PEBBLE_SCREEN_WIDTH-20,lineHeight));
  s_phone_battery_layer = text_layer_create(GRect(PEBBLE_SCREEN_WIDTH/3, level3, PEBBLE_SCREEN_WIDTH-PEBBLE_SCREEN_WIDTH/3, lineHeight));
  s_network_layer = text_layer_create(GRect(0, level3, PEBBLE_SCREEN_WIDTH/3, lineHeight));
  s_weather_layer = text_layer_create(GRect(0, level4, PEBBLE_SCREEN_WIDTH,lineHeight));

  applyBlackStyle(s_bt_layer);
  applyBlackStyle(s_time_layer);
  applyWhiteStyle(s_battery_layer);
  applyWhiteStyle(s_date_layer);
  applyBlackStyle(s_phone_battery_layer);
  applyWhiteStyle(s_network_layer);
  applyBlackStyle(s_weather_layer);

  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_battery_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_phone_battery_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_network_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_weather_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_bt_layer));

  //handle btconnection
  bluetooth_connection_service_subscribe(bluetoothHandler);
  bluetoothHandler(bluetooth_connection_service_peek());

  // handle time
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  pushTimeToLayer(s_time_layer);

  // handle pebble batterry
  battery_state_service_subscribe(battery_handler);
  battery_handler(battery_state_service_peek());
  
  batteryStatus = persist_read_int(KEY_BATTERY_CHARGING);
  batteryLevel = persist_read_int(KEY_BATTERY_LEVEL);
  pushPhoneBatteryToLayout(s_phone_battery_layer,batteryLevel,batteryStatus);
  
  networkStatus = persist_read_int(KEY_NETWORK);
  pushPhoneNetworkStatusToLayout(s_network_layer,networkStatus);
  
  static char date[16];
  persist_read_string(KEY_DATE,date,sizeof(date));
  text_layer_set_text(s_date_layer,date);
  
  static char weather[16];
  persist_read_string(KEY_WEATHER,weather,sizeof(weather));
  text_layer_set_text(s_weather_layer,weather);
}

static void main_window_unload(Window *window) {
  // Destroy output TextLayer
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_battery_layer);
  text_layer_destroy(s_phone_battery_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_network_layer);
  text_layer_destroy(s_weather_layer);
  text_layer_destroy(s_bt_layer);  
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