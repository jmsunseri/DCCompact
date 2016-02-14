#include <pebble.h>


#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1
#define KEY_BACKGROUND_COLOR 2
#define KEY_TWENTY_FOUR_HOUR_FORMAT 3

static Window *s_main_window;
static TextLayer *s_time_layer_minute;
static TextLayer *s_time_layer_hour;
static TextLayer *s_time_layer_date;
static TextLayer *s_weather_layer;
static TextLayer *s_weather2_layer;

static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;


static GFont s_time2_font;
static GFont s_time_font;
static GFont s_time4_font;
static GFont s_weather_font;
static GFont s_weather2_font;

static bool twenty_four_hour_format = false;

static void set_colors(int background_color) {
    GColor bg_color = GColorFromHEX(background_color);
    window_set_background_color(s_main_window, bg_color);
    //text_layer_set_text_color()
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  

  // Write the current hours and minutes into a buffer
  static char s_buffer_hour[8];
  static char s_buffer_minute[8];
  static char date_buffer[16];
  strftime(date_buffer, sizeof(date_buffer), "%d", tick_time);
  
  if(clock_is_24h_style() == twenty_four_hour_format){
      strftime(s_buffer_hour, sizeof(s_buffer_hour), "%H", tick_time);
  } else {
      strftime(s_buffer_hour, sizeof(s_buffer_hour), "%I", tick_time);
  }
  
  
  //strftime(s_buffer_hour, sizeof(s_buffer_hour), clock_is_24h_style() ?
  //                                        "%H" : "%I", tick_time);
  strftime(s_buffer_minute, sizeof(s_buffer_minute), clock_is_24h_style() ?
                                          "%M" : "%M", tick_time);
  
  

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer_hour, s_buffer_hour);
  text_layer_set_text(s_time_layer_minute, s_buffer_minute);
  text_layer_set_text(s_time_layer_date, date_buffer);
}


static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Store incoming information
  static char temperature_buffer[8];
  static char weather_layer_buffer[32];
  static char weather_layer2_buffer[32];
  static char conditions_buffer[32];
  
  //options from configuration
  
    APP_LOG(APP_LOG_LEVEL_INFO, "Inbox Callback Received");

  // Read tuples for data
  Tuple *temp_tuple = dict_find(iterator, KEY_TEMPERATURE);
  Tuple *conditions_tuple = dict_find(iterator, KEY_CONDITIONS);
  Tuple *twenty_four_hour_format_tuple = dict_find(iterator, KEY_TWENTY_FOUR_HOUR_FORMAT);
  Tuple *background_color_tuple = dict_find(iterator, KEY_BACKGROUND_COLOR);

  if(background_color_tuple){
      int background_color = background_color_tuple->value->int32;
      
      APP_LOG(APP_LOG_LEVEL_INFO, "Background color detected");
      persist_write_int(KEY_BACKGROUND_COLOR, background_color);
      set_colors(background_color);
  }
  
  if(twenty_four_hour_format_tuple){
      twenty_four_hour_format = twenty_four_hour_format_tuple->value->int8;
      persist_write_int(KEY_TWENTY_FOUR_HOUR_FORMAT, twenty_four_hour_format);
      update_time();
  }


  // If all data is available, use it
  if(temp_tuple && conditions_tuple) {
    snprintf(temperature_buffer, sizeof(temperature_buffer), "%d", (int)temp_tuple->value->int32);
    snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", conditions_tuple->value->cstring);

    
    
    // Assemble full string and display
    snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s", temperature_buffer);
    text_layer_set_text(s_weather_layer, weather_layer_buffer);
    snprintf(weather_layer2_buffer, sizeof(weather_layer2_buffer), "%s", conditions_buffer);
    text_layer_set_text(s_weather2_layer, weather_layer2_buffer);
    
    
    
  }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}



static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();

  // Get weather update every 15 minutes
  if(tick_time->tm_min % 15 == 0) {
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);

    // Send the message!
    app_message_outbox_send();
  }
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create GBitmap
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);

  // Create BitmapLayer to display the GBitmap
  s_background_layer = bitmap_layer_create(bounds);

  // Set the bitmap onto the layer and add to the window
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));


  

 // Hour Layer
  
     // Create the TextLayer with specific bounds
  s_time_layer_hour = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(20, 20), bounds.size.w, 50));

  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer_hour, GColorClear);
  text_layer_set_text_color(s_time_layer_hour, GColorWhite);
  text_layer_set_text(s_time_layer_hour, "0000");
  text_layer_set_text_alignment(s_time_layer_hour, GTextAlignmentCenter);
  
  // Create GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_50));

  // Apply to TextLayer
  text_layer_set_font(s_time_layer_hour, s_time_font);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer_hour));
  
  
  
  
  
  // Minutes Layer
  
     // Create the TextLayer with specific bounds
  s_time_layer_minute = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(55, 55), bounds.size.w, 50));

  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer_minute, GColorClear);
  text_layer_set_text_color(s_time_layer_minute, GColorYellow);
  text_layer_set_text(s_time_layer_minute, "0000");
  text_layer_set_text_alignment(s_time_layer_minute, GTextAlignmentCenter);
  
  // Create GFont
  s_time2_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_50));

  // Apply to TextLayer
  text_layer_set_font(s_time_layer_minute, s_time2_font);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer_minute));
  
   

     // Third text layer for Date
  
     // Create the TextLayer with specific bounds
  s_time_layer_date = text_layer_create(
      GRect(PBL_IF_ROUND_ELSE(-54,-38), PBL_IF_ROUND_ELSE(101, 125), bounds.size.w, 50));

  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer_date, GColorClear);
  text_layer_set_text_color(s_time_layer_date, GColorWhite);
  text_layer_set_text(s_time_layer_date, "0000");
  text_layer_set_text_alignment(s_time_layer_date, GTextAlignmentCenter);
  
  // Create GFont
  s_time4_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_19));

  // Apply to TextLayer
  text_layer_set_font(s_time_layer_date, s_time4_font);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer_date));
 
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  // Create temperature Layer
  s_weather_layer = text_layer_create(
      GRect(PBL_IF_ROUND_ELSE(-61,-49), PBL_IF_ROUND_ELSE(78, 103), bounds.size.w, 100));

  // Style the text
  text_layer_set_background_color(s_weather_layer, GColorClear);
  text_layer_set_text_color(s_weather_layer, GColorBlack);
  text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);
  text_layer_set_text(s_weather_layer, "DC");

  // Create second custom font, apply it and add to Window
  s_weather_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_15));
  text_layer_set_font(s_weather_layer, s_weather_font);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_layer));

  
   // Create temperature2 Layer
  s_weather2_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(100, 100), bounds.size.w, 50));

  // Style the text
  text_layer_set_background_color(s_weather2_layer, GColorClear);
  text_layer_set_text_color(s_weather2_layer, GColorWhite);
  text_layer_set_text_alignment(s_weather2_layer, GTextAlignmentCenter);
  text_layer_set_text(s_weather2_layer, "ERROR");

  // Create second custom font, apply it and add to Window
  s_weather2_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_25));
  text_layer_set_font(s_weather2_layer, s_weather2_font);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather2_layer));
  
 
  if(persist_read_int(KEY_BACKGROUND_COLOR)){
      int background_color = persist_read_int(KEY_BACKGROUND_COLOR);
      set_colors(background_color);
  }
  
  if(persist_read_bool(KEY_TWENTY_FOUR_HOUR_FORMAT)){
      twenty_four_hour_format = persist_read_bool(KEY_TWENTY_FOUR_HOUR_FORMAT);
      
      update_time();
  }

}




static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer_hour);
  text_layer_destroy(s_time_layer_minute);
  text_layer_destroy(s_time_layer_date);
  
  // Unload GFont
  fonts_unload_custom_font(s_time_font);
  fonts_unload_custom_font(s_time2_font);
  fonts_unload_custom_font(s_time4_font);

  // Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);

  // Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);


  // Destroy weather elements
  text_layer_destroy(s_weather_layer);
  fonts_unload_custom_font(s_weather_font);
  text_layer_destroy(s_weather2_layer);
  fonts_unload_custom_font(s_weather2_font);
  
}


static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set the background color
  window_set_background_color(s_main_window, GColorBlack);

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);

  // Make sure the time is displayed from the start
  update_time();

  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);

  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}





static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}