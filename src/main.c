#include <pebble.h>
#define TEXT_COLOR 0
static GColor text_color;
#define BG_COLOR 1
static GColor bg_color;

static Window *s_main_window;
static TextLayer *s_time_layer;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[10];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H\n%M\n%S" : "%I\n%M\n%S %p", tick_time);

  // Display this time on the TextLayer
  window_set_background_color(s_main_window, bg_color);
  text_layer_set_background_color(s_time_layer, bg_color);
  text_layer_set_text_color(s_time_layer, text_color);
  text_layer_set_text(s_time_layer, s_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void main_window_load(Window *window) {
  static GFont s_font;
  
  if (persist_exists(TEXT_COLOR)) {
      int color = persist_read_int(TEXT_COLOR);
      text_color = GColorFromHEX(color);
  } else {
    text_color = GColorWhite;
  }
  if (persist_exists(BG_COLOR)) {
     int color = persist_read_int(BG_COLOR);
     bg_color = GColorFromHEX(color);
  } else {
    bg_color = GColorBlack;
  }
  
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create the TextLayer with specific bounds
  s_time_layer = text_layer_create(GRect(3, 0, bounds.size.w, bounds.size.h));
  s_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BASIC_DOTS_52));
  
  // Improve the layout to be more like a watchface
  window_set_background_color(s_main_window, bg_color);
  text_layer_set_background_color(s_time_layer, bg_color);
  text_layer_set_text_color(s_time_layer, text_color);
  text_layer_set_text(s_time_layer, "00\n00\n00");
  text_layer_set_font(s_time_layer, s_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *text_color_t = dict_find(iter, TEXT_COLOR);
  Tuple *bg_color_t = dict_find(iter, BG_COLOR);

  if (text_color_t) {
    int text_color_int = text_color_t->value->int32;
    persist_write_int(TEXT_COLOR, text_color_int);
    text_color = GColorFromHEX(text_color_int);
  }

  if (bg_color_t) {
      int bg_color_int = bg_color_t->value->int32;
      persist_write_int(BG_COLOR, bg_color_int);
      bg_color = GColorFromHEX(bg_color_int);
    }
}

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  

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
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  
  app_message_register_inbox_received(inbox_received_handler);
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