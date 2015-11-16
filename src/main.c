#include <pebble.h>
  
static Window *s_main_window;
static TextLayer *s_date_layer, *s_day_layer, *s_month_layer;
static GFont s_big_font, s_small_font;

static void update_time() {
  
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char date_buffer[] = "00";
  static char day_buffer[40];
  static char month_buffer[40];
  
  // Write the current date
  strftime(date_buffer, sizeof(date_buffer), "%e", tick_time);
  strftime(day_buffer, sizeof(day_buffer), "%A", tick_time);
  strftime(month_buffer, sizeof(month_buffer), "%B", tick_time);

  // Display this on the TextLayer
  text_layer_set_text(s_date_layer, date_buffer);
  text_layer_set_text(s_day_layer, day_buffer);
  text_layer_set_text(s_month_layer, month_buffer);

}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void main_window_load(Window *window) {
  
  // Set Locale
  setlocale(LC_ALL, i18n_get_system_locale());
  
  // Create GFont
  s_big_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_KEY_GOTHAM_78));
  s_small_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_KEY_GOTHAM_32));
  
  // Create DAY TextLayer
  s_day_layer = text_layer_create(GRect(0,0,144,38));
  text_layer_set_background_color(s_day_layer, GColorWhite);
#ifdef PBL_COLOR
  text_layer_set_text_color(s_day_layer, GColorDarkGray);
#else
  text_layer_set_text_color(s_day_layer, GColorBlack);
#endif
  
  // Improve the layout to be more like a watchface
  text_layer_set_font(s_day_layer, s_small_font);
  text_layer_set_text_alignment(s_day_layer, GTextAlignmentCenter);
  
  // Create DATE TextLayer
  s_date_layer = text_layer_create(GRect(0, 38, 144, 78));
  text_layer_set_background_color(s_date_layer, GColorWhite);
#ifdef PBL_COLOR
  text_layer_set_text_color(s_date_layer, GColorCobaltBlue);
#else
  text_layer_set_text_color(s_date_layer, GColorBlack);
#endif
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);

  // Improve the layout to be more like a watchface
  text_layer_set_font(s_date_layer, s_big_font);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);

  // Create MONTH TextLayer
  s_month_layer = text_layer_create(GRect(0,130,144,38));
  text_layer_set_background_color(s_month_layer, GColorWhite);
#ifdef PBL_COLOR
  text_layer_set_text_color(s_month_layer, GColorDarkGray);
#else
  text_layer_set_text_color(s_month_layer, GColorBlack);
#endif
  
  // Improve the layout to be more like a watchface
  text_layer_set_font(s_month_layer, s_small_font);
  text_layer_set_text_alignment(s_month_layer, GTextAlignmentCenter);
  
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_day_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_month_layer));
}

static void main_window_unload(Window *window) {
  
  // Unload GFont
  fonts_unload_custom_font(s_big_font);
  fonts_unload_custom_font(s_small_font);
  
  // Destroy TextLayer
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_day_layer);
  text_layer_destroy(s_month_layer);

}

static void init() {
  
  // Register with TickTimerService
  tick_timer_service_subscribe(DAY_UNIT, tick_handler);
  
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  
  // Full screen for Pebble
#ifdef PBL_PLATFORM_APLITE
  window_set_fullscreen(s_main_window, true);
#endif
  
  window_set_background_color(s_main_window, GColorWhite);

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Make sure the time is displayed from the start
  update_time();
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