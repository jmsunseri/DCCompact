#include <pebble.h>



#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1
#define KEY_BACKGROUND_COLOR 2
#define KEY_TWENTY_FOUR_HOUR_FORMAT 3
#define KEY_HOUR_COLOR 4
#define KEY_METRIC_UNITS 5
#define KEY_MINUTE_COLOR 6
#define KEY_DATE_TEXT_COLOR 7
#define KEY_TEMP_TEXT_COLOR 8
#define KEY_CONDITIONS_COLOR 9

static Window *s_main_window;
static TextLayer *s_time_layer_minute;
static TextLayer *s_time_layer_hour;
static TextLayer *s_date_layer;
static TextLayer *s_temp_layer;
static TextLayer *s_conditions_layer;

static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;


static GFont s_time2_font;
static GFont s_time_font;
static GFont s_time4_font;
static GFont s_temp_font;
static GFont s_conditions_font;

static bool twenty_four_hour_format = false;
static bool metric_units = true;
static int temp_in_c = 0;

static void set_colors(GColor background_color, GColor hour_color, GColor minute_color,
  GColor date_text_color, GColor temp_text_color, GColor conditions_color ) {  
  //APP_LOG(APP_LOG_LEVEL_INFO, "background color hex: %d", background_color);

  //GColor bg_color = GColorFromHEX(background_color);
  window_set_background_color(s_main_window, background_color);
  //text_layer_set_text_color()

  //setting the color for the hour text layer
  text_layer_set_background_color(s_time_layer_hour, GColorClear);
  text_layer_set_text_color(s_time_layer_hour, hour_color);

  //setting the color for the minute text layer
  text_layer_set_background_color(s_time_layer_minute, GColorClear);
  text_layer_set_text_color(s_time_layer_minute, minute_color);

  //setting the color for the date text layer
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, date_text_color);

  //setting the color for the weather layer
  text_layer_set_background_color(s_temp_layer, GColorClear);
  text_layer_set_text_color(s_temp_layer, temp_text_color);

  //setting the color for the conditions layer
  text_layer_set_background_color(s_conditions_layer, GColorClear);
  text_layer_set_text_color(s_conditions_layer, conditions_color);
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
  
  strftime(s_buffer_minute, sizeof(s_buffer_minute), clock_is_24h_style() ?
                                          "%M" : "%M", tick_time);
  
  

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer_hour, s_buffer_hour);
  text_layer_set_text(s_time_layer_minute, s_buffer_minute);
  text_layer_set_text(s_date_layer, date_buffer);
}

static void update_temperature() {
  static char temperature_buffer[8];
  static char temp_layer_buffer[32];
  
  if(metric_units) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Using metric units");
    snprintf(temperature_buffer, sizeof(temperature_buffer), "%d", temp_in_c);
  }
  else {
    APP_LOG(APP_LOG_LEVEL_INFO, "Using imperial units");
    snprintf(temperature_buffer, sizeof(temperature_buffer), "%d", (int)(temp_in_c *1.8 + 32));
  }
  
  snprintf(temp_layer_buffer, sizeof(temp_layer_buffer), "%s", temperature_buffer);
  text_layer_set_text(s_temp_layer, temp_layer_buffer);
}




static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Store incoming information
  
  static char conditions_layer_buffer[32];
  static char conditions_buffer[32];
  
  //options from configuration
  
  APP_LOG(APP_LOG_LEVEL_INFO, "Inbox Callback Received");

  // Read tuples for data
  Tuple *temp_tuple = dict_find(iterator, KEY_TEMPERATURE);
  Tuple *conditions_tuple = dict_find(iterator, KEY_CONDITIONS);
  Tuple *twenty_four_hour_format_tuple = dict_find(iterator, KEY_TWENTY_FOUR_HOUR_FORMAT);
  Tuple *background_color_tuple = dict_find(iterator, KEY_BACKGROUND_COLOR);
  Tuple *hour_color_tuple = dict_find(iterator, KEY_HOUR_COLOR);
  Tuple *minute_color_tuple = dict_find(iterator, KEY_MINUTE_COLOR);
  Tuple *date_text_color_tuple = dict_find(iterator, KEY_DATE_TEXT_COLOR);
  Tuple *temp_text_color_tuple = dict_find(iterator, KEY_TEMP_TEXT_COLOR);
  Tuple *conditions_color_tuple = dict_find(iterator, KEY_CONDITIONS_COLOR);
  Tuple *metric_units_tuple = dict_find(iterator, KEY_METRIC_UNITS);

  if(background_color_tuple){
    APP_LOG(APP_LOG_LEVEL_INFO, "Background color tuple detected!  Must be an update from the config page.");
    
    int background_color = background_color_tuple->value->int32;
    persist_write_int(KEY_BACKGROUND_COLOR, background_color);
    
    int hour_color = hour_color_tuple->value->int32;
    persist_write_int(KEY_HOUR_COLOR, hour_color);
    
    int minute_color = minute_color_tuple->value->int32;
    persist_write_int(KEY_MINUTE_COLOR, minute_color);
    
    int date_text_color = date_text_color_tuple->value->int32;
    persist_write_int(KEY_DATE_TEXT_COLOR, date_text_color);
    
    int temp_text_color = temp_text_color_tuple->value->int32;
    persist_write_int(KEY_DATE_TEXT_COLOR, temp_text_color);
    
    int conditions_color = conditions_color_tuple->value->int32;
    persist_write_int(KEY_CONDITIONS_COLOR, conditions_color);
      
    set_colors(GColorFromHEX(background_color),GColorFromHEX(hour_color), GColorFromHEX(minute_color), 
      GColorFromHEX(date_text_color), GColorFromHEX(temp_text_color), GColorFromHEX(conditions_color));
  }
  
  if(twenty_four_hour_format_tuple){
    twenty_four_hour_format = twenty_four_hour_format_tuple->value->int8;
    persist_write_int(KEY_TWENTY_FOUR_HOUR_FORMAT, twenty_four_hour_format);
    update_time();
  }

  if(metric_units_tuple){
    APP_LOG(APP_LOG_LEVEL_INFO, "Metric unit tuple detected");
    
    if(metric_units != metric_units_tuple->value->int8){
      APP_LOG(APP_LOG_LEVEL_INFO, "Change in temp configuration detected");
      metric_units = metric_units_tuple->value->int8;    
      persist_write_int(KEY_METRIC_UNITS, metric_units);
      
      update_temperature();
    }
    else {
      APP_LOG(APP_LOG_LEVEL_INFO, "No change in temp configuration detected");
    }
    
  }
  

  // If all data is available, use it
  if(temp_tuple && conditions_tuple) {
    temp_in_c = (int)temp_tuple->value->int32;
    
    update_temperature();
    
    //if(metric_units){
    //  APP_LOG(APP_LOG_LEVEL_INFO, "Using metric units");
    //  snprintf(temperature_buffer, sizeof(temperature_buffer), "%d", temp_in_c);
    //}
    //else {
    //  APP_LOG(APP_LOG_LEVEL_INFO, "Using imperial units");
    //  snprintf(temperature_buffer, sizeof(temperature_buffer), "%d", (int)((temp_in_c *1.8 + 32)   ));
    //}
    
    
    snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", conditions_tuple->value->cstring);
 
    // Assemble full string and display
    //snprintf(temp_layer_buffer, sizeof(temp_layer_buffer), "%s", temperature_buffer);
    //text_layer_set_text(s_temp_layer, temp_layer_buffer);
    snprintf(conditions_layer_buffer, sizeof(conditions_layer_buffer), "%s", conditions_buffer);
    text_layer_set_text(s_conditions_layer, conditions_layer_buffer);
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

  // Create Background
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
  s_background_layer = bitmap_layer_create(bounds);
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));

  // Hour Layer
  s_time_layer_hour = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(20, 20), bounds.size.w, 50));
  text_layer_set_text(s_time_layer_hour, "0000");
  text_layer_set_text_alignment(s_time_layer_hour, GTextAlignmentCenter);
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_50));
  text_layer_set_font(s_time_layer_hour, s_time_font);
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer_hour));

  // Minutes Layer
  s_time_layer_minute = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(55, 55), bounds.size.w, 50));
  text_layer_set_text(s_time_layer_minute, "0000");
  text_layer_set_text_alignment(s_time_layer_minute, GTextAlignmentCenter);
  s_time2_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_50));
  text_layer_set_font(s_time_layer_minute, s_time2_font);
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer_minute));

  //Date Layer
  s_date_layer = text_layer_create(
  GRect(PBL_IF_ROUND_ELSE(-54,-38), PBL_IF_ROUND_ELSE(101, 125), bounds.size.w, 50));
  text_layer_set_text(s_date_layer, "0000");
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  s_time4_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_19));
  text_layer_set_font(s_date_layer, s_time4_font);
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));

  //Temp Layer
  s_temp_layer = text_layer_create(
  GRect(PBL_IF_ROUND_ELSE(-61,-49), PBL_IF_ROUND_ELSE(78, 103), bounds.size.w, 100));
  text_layer_set_text_alignment(s_temp_layer, GTextAlignmentCenter);
  text_layer_set_text(s_temp_layer, "DC");
  s_temp_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_15));
  text_layer_set_font(s_temp_layer, s_temp_font);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_temp_layer));


  //Conditions Layer
  s_conditions_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(100, 100), bounds.size.w, 50));
  text_layer_set_text_alignment(s_conditions_layer, GTextAlignmentCenter);
  text_layer_set_text(s_conditions_layer, "???");
  s_conditions_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_25));
  text_layer_set_font(s_conditions_layer, s_conditions_font);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_conditions_layer));

  //Setting the default colors in case there is nothing in persistent storage
  GColor background_color = GColorBlack;
  GColor hour_color = GColorWhite;
  GColor minute_color = GColorYellow;
  GColor date_text_color = GColorWhite;
  GColor temp_text_color = GColorBlack;
  GColor conditions_color = GColorWhite;

  if(persist_read_int(KEY_BACKGROUND_COLOR)){
    background_color = GColorFromHEX(persist_read_int(KEY_BACKGROUND_COLOR));
  }
  if(persist_read_int(KEY_HOUR_COLOR)){
    hour_color = GColorFromHEX(persist_read_int(KEY_HOUR_COLOR));
  }
  if(persist_read_int(KEY_MINUTE_COLOR)){
    minute_color = GColorFromHEX(persist_read_int(KEY_MINUTE_COLOR));
  }
  
  if(persist_read_int(KEY_DATE_TEXT_COLOR)){
    date_text_color = GColorFromHEX(persist_read_int(KEY_DATE_TEXT_COLOR));
  }
  
  if(persist_read_int(KEY_TEMP_TEXT_COLOR)){
    temp_text_color = GColorFromHEX(persist_read_int(KEY_TEMP_TEXT_COLOR));
  }
  
  if(persist_read_int(KEY_CONDITIONS_COLOR)){
    conditions_color = GColorFromHEX(persist_read_int(KEY_CONDITIONS_COLOR));
  }
    
  set_colors(background_color, hour_color, minute_color, date_text_color, 
    temp_text_color, conditions_color);
  
  if(persist_read_bool(KEY_METRIC_UNITS)){
    metric_units = persist_read_bool(KEY_METRIC_UNITS);
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
  text_layer_destroy(s_date_layer);
  
  // Unload GFont
  fonts_unload_custom_font(s_time_font);
  fonts_unload_custom_font(s_time2_font);
  fonts_unload_custom_font(s_time4_font);

  // Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);

  // Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);


  // Destroy weather elements
  text_layer_destroy(s_temp_layer);
  fonts_unload_custom_font(s_temp_font);
  text_layer_destroy(s_conditions_layer);
  fonts_unload_custom_font(s_conditions_font);
  
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