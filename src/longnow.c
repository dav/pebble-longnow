#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0xD4, 0x84, 0x91, 0x6A, 0x37, 0x28, 0x44, 0x06, 0xA2, 0x26, 0x45, 0xCA, 0x82, 0x63, 0x5E, 0x35 }
PBL_APP_INFO(MY_UUID,
             "The Long Now", "Dav Yaginuma",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

// Views
Window window;
TextLayer text_year_layer;
TextLayer text_date_layer;
TextLayer text_time_layer;
TextLayer text_title_layer;
Layer line_layer;

void line_layer_update_callback(Layer *me, GContext* ctx) {
  (void)me;
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_draw_line(ctx, GPoint(8, 97), GPoint(131, 97));
  graphics_draw_line(ctx, GPoint(8, 98), GPoint(131, 98));
}

void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Long Now");
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, GColorBlack);

  resource_init_current_app(&APP_RESOURCES);

  text_layer_init(&text_title_layer, window.layer.frame);
  text_layer_set_text_color(&text_title_layer, GColorWhite);
  text_layer_set_background_color(&text_title_layer, GColorClear);
  layer_set_frame(&text_title_layer.layer, GRect(8, 12, 144-8, 25));
  text_layer_set_font(&text_title_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21)));
  layer_add_child(&window.layer, &text_title_layer.layer);
  text_layer_set_text(&text_title_layer, "The Long Now");

  text_layer_init(&text_year_layer, window.layer.frame);
  text_layer_set_text_color(&text_year_layer, GColorWhite);
  text_layer_set_background_color(&text_year_layer, GColorClear);
  layer_set_frame(&text_year_layer.layer, GRect(8, 37, 144-8, 22));
  text_layer_set_font(&text_year_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21)));
  layer_add_child(&window.layer, &text_year_layer.layer);

  text_layer_init(&text_date_layer, window.layer.frame);
  text_layer_set_text_color(&text_date_layer, GColorWhite);
  text_layer_set_background_color(&text_date_layer, GColorClear);
  layer_set_frame(&text_date_layer.layer, GRect(8, 68, 144-8, 168-68));
  text_layer_set_font(&text_date_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21)));
  layer_add_child(&window.layer, &text_date_layer.layer);

  text_layer_init(&text_time_layer, window.layer.frame);
  text_layer_set_text_color(&text_time_layer, GColorWhite);
  text_layer_set_background_color(&text_time_layer, GColorClear);
  layer_set_frame(&text_time_layer.layer, GRect(7, 92, 144-7, 168-92));
  text_layer_set_font(&text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_SUBSET_49)));
  layer_add_child(&window.layer, &text_time_layer.layer);


  layer_init(&line_layer, window.layer.frame);
  line_layer.update_proc = &line_layer_update_callback;
  layer_add_child(&window.layer, &line_layer);

  // TODO: Update display here to avoid blank display on launch?
}

void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
  (void)ctx;

  // Need to be static because they're used by the system later.
  static char year_text[] = "00000";
  static char date_text[] = "Xxxxxxxxx 00";
  static char time_text[] = "00:00";

  char *time_format;

  // Display Year
  string_format_time(year_text, sizeof(year_text), "0%Y", t->tick_time);
  text_layer_set_text(&text_year_layer, year_text);

  // Display Date
  // TODO: Only update the date when it's changed.
  string_format_time(date_text, sizeof(date_text), "%B %e", t->tick_time);
  text_layer_set_text(&text_date_layer, date_text);

  // Display Time
  if (clock_is_24h_style()) {
    time_format = "%R";
  } else {
    time_format = "%I:%M";
  }
  string_format_time(time_text, sizeof(time_text), time_format, t->tick_time);
  // Kludge to handle lack of non-padded hour format string
  // for twelve hour clock.
  if (!clock_is_24h_style() && (time_text[0] == '0')) {
    memmove(time_text, &time_text[1], sizeof(time_text) - 1);
  }
  text_layer_set_text(&text_time_layer, time_text);
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,

    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = MINUTE_UNIT
    }

  };
  app_event_loop(params, &handlers);
}
