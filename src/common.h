#include <pebble.h>
#include <constants.h>

// handle current time
static void pushTimeToLayer(TextLayer *time_layer,TextLayer *date_layer) {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  static char buffer[16];
  static char buffer_date[64];

  if(clock_is_24h_style() == true) {
    strftime(buffer, sizeof(buffer), "%H:%M", tick_time);
  } else {
    strftime(buffer, sizeof(buffer), "%I:%M", tick_time);
  }
  text_layer_set_text(time_layer,buffer);
  
  strftime(buffer_date, sizeof(buffer_date), "%A(%u)%n %d %B(%m)", tick_time);
  text_layer_set_text(date_layer,buffer_date);  
}

// handle current watch batterry status
static void pushBatteryStateToLayout(BatteryChargeState charge_state,TextLayer *battery_layer){
  static char s_battery_buffer[16];
  if (charge_state.is_charging) {
    snprintf(s_battery_buffer, sizeof(s_battery_buffer), "charging");
  } else {
    snprintf(s_battery_buffer, sizeof(s_battery_buffer), "%d%%", charge_state.charge_percent);
  }
  text_layer_set_text(battery_layer, s_battery_buffer);
}

// push phone battery status
static void pushPhoneBatteryToLayout(TextLayer *battery_layer,int batteryLevel,int batteryStatus){
  static char s_battery_buffer[16];
  switch(batteryStatus){
    case KEY_BATTERY_CHARGING_NONE:
      snprintf(s_battery_buffer, sizeof(s_battery_buffer), "%d%%(-)", batteryLevel);
      break;
    case KEY_BATTERY_CHARGING_USB:
      snprintf(s_battery_buffer, sizeof(s_battery_buffer), "%d%%(usb)", batteryLevel);
      break;
    case KEY_BATTERY_CHARGING_SET:
      snprintf(s_battery_buffer, sizeof(s_battery_buffer), "%d%%(ac)", batteryLevel);
      break;
    case KEY_BATTERY_CHARGING_WIRELESS:
      snprintf(s_battery_buffer, sizeof(s_battery_buffer), "%d%%(qi)", batteryLevel);
      break;
  }
  text_layer_set_text(battery_layer, s_battery_buffer);
}

static void pushPhoneNetworkStatusToLayout(TextLayer *s_network_layer,int networkStatus){
  static char s_network_buffer[16];
  switch(networkStatus){
    case KEY_NETWORK_OFF:
      snprintf(s_network_buffer, sizeof(s_network_buffer), "off");
      break;
    case KEY_NETWORK_WIFI:
      snprintf(s_network_buffer, sizeof(s_network_buffer), "wifi");
      break;
    case KEY_NETWORK_WIFI_MOBILE:
      snprintf(s_network_buffer, sizeof(s_network_buffer), "mb/wf");
      break;
    case KEY_NETWORK_MOBILE:
      snprintf(s_network_buffer, sizeof(s_network_buffer), "mb");
      break;
  }
  text_layer_set_text(s_network_layer, s_network_buffer);
}

// sytle layout as black layout with white text
static void applyBlackStyle(TextLayer *s_layer){
  text_layer_set_background_color(s_layer, GColorBlack);
  text_layer_set_text_color(s_layer, GColorWhite);
  text_layer_set_font(s_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(s_layer, GTextAlignmentCenter);
}

// sytle layout as white background with black text
static void applyWhiteStyle(TextLayer *s_layer){
  text_layer_set_background_color(s_layer, GColorWhite);
  text_layer_set_text_color(s_layer, GColorBlack);
  text_layer_set_font(s_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(s_layer, GTextAlignmentCenter);  
}