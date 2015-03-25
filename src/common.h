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

static uint32_t getPebbleBatteryImageResource(BatteryChargeState charge_state){
  if (charge_state.is_charging) {
    return RESOURCE_ID_IMAGE_PBL_CHARGING;
  } else {
    switch(charge_state.charge_percent/10){
      case 10:
        return RESOURCE_ID_IMAGE_PBL_100;
      case 9:
        return RESOURCE_ID_IMAGE_PBL_90;
      case 8:
        return RESOURCE_ID_IMAGE_PBL_80;
      case 7:
        return RESOURCE_ID_IMAGE_PBL_70;
      case 6:
        return RESOURCE_ID_IMAGE_PBL_60;
      case 5:
        return RESOURCE_ID_IMAGE_PBL_50;
      case 4:
        return RESOURCE_ID_IMAGE_PBL_40;
      case 3:
        return RESOURCE_ID_IMAGE_PBL_30;
      case 2:
        return RESOURCE_ID_IMAGE_PBL_20;
    }
  }
  // return 10% by default
  return RESOURCE_ID_IMAGE_PBL_10;
}

// push phone battery status
static void pushPhoneBatteryToLayout(InverterLayer *inverterlayer_phone_battery_level,TextLayer *battery_layer,int batteryLevel,int batteryStatus,bool phoneConnected){
  static char s_battery_buffer[16];
  if(phoneConnected){
    static int newHeight;
    newHeight = (int)(batteryLevel * ((double)TRANSPARENT_BATTERY_H / (double )KEY_BATTERY_LEVEL_MAX));
    GRect bounds = layer_get_bounds(inverter_layer_get_layer (inverterlayer_phone_battery_level));
    bounds.size.h = newHeight;
    bounds.origin.y = TRANSPARENT_BATTERY_H - newHeight;// strange logic here
    layer_set_bounds(inverter_layer_get_layer (inverterlayer_phone_battery_level),bounds);

    switch(batteryStatus){
      case KEY_BATTERY_CHARGING_NONE:
        snprintf(s_battery_buffer, sizeof(s_battery_buffer), "%d%%", batteryLevel);
        break;
      case KEY_BATTERY_CHARGING_USB:
        snprintf(s_battery_buffer, sizeof(s_battery_buffer), "%d%% (usb)", batteryLevel);
        break;
      case KEY_BATTERY_CHARGING_SET:
        snprintf(s_battery_buffer, sizeof(s_battery_buffer), "%d%% (ac)", batteryLevel);
        break;
      case KEY_BATTERY_CHARGING_WIRELESS:
        snprintf(s_battery_buffer, sizeof(s_battery_buffer), "%d%% (qi)", batteryLevel);
        break;
    }
  }else{
    snprintf(s_battery_buffer, sizeof(s_battery_buffer), "OFF");
  }
  text_layer_set_text(battery_layer, s_battery_buffer);
}

static void pushPhoneNetworkStatusToLayout(BitmapLayer *s_bitmaplayer_network,GBitmap *s_res_image_phone_network,int networkStatus){  
  switch(networkStatus){
    case KEY_NETWORK_OFF:
      s_res_image_phone_network = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PHONE_NETWORK_NO);
      break;
    case KEY_NETWORK_WIFI:
      s_res_image_phone_network = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PHONE_NETWORK_WIFI);
      break;
    case KEY_NETWORK_MOBILE:
      s_res_image_phone_network = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PHONE_NETWORK_MOBILE);
      break;
  }
  bitmap_layer_set_bitmap(s_bitmaplayer_network, s_res_image_phone_network);
}