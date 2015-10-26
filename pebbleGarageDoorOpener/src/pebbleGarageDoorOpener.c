#include <pebble.h>

#define ENABLE_CLOCK_APP 0
#define ENABLE_GARAGE_DOOR_APP 1

static Window *s_main_window;
static TextLayer *s_text_layer;

static void main_window_load(Window *window);
static void main_window_unload(Window *window);
static void init();
static void deinit();
#if ENABLE_CLOCK_APP
static void tick_handler(struct tm *tick_time, TimeUnits units_changed);
static void update_time();
#endif //ENABLE_CLOCK_APP

#if ENABLE_GARAGE_DOOR_APP
void config_provider(Window *window);
void down_single_click_handler(ClickRecognizerRef recognizer, void *context);
void up_single_click_handler(ClickRecognizerRef recognizer, void *context);
void select_single_click_handler(ClickRecognizerRef recognizer, void *context);
#endif //ENABLE_GARAGE_DOOR_APP

int main(void)
{
    init();
    app_event_loop();
    deinit();
}

static void init()
{
    //create main window element and assign to pointer
    s_main_window = window_create();
    
    //set handlers to manage the elements inside the window
    window_set_window_handlers(s_main_window, (WindowHandlers){
        .load = main_window_load,
        .unload = main_window_unload 
    });
   
#if ENABLE_GARAGE_DOOR_APP
    //Register clickConfigProvider for the window
    window_set_click_config_provider(s_main_window, (ClickConfigProvider)config_provider);     
#endif //ENABLE_GARAGE_DOOR_APP 
    
    //Show the window on the watch, with animated=true
    window_stack_push(s_main_window, true);

#if ENABLE_CLOCK_APP    
    //Make sure the time is displayed from start
    update_time();    
    //Register with TickTimerService
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
#endif //ENABLE_CLOCK_APP
}

static void deinit()
{
    //destroy window
    window_destroy(s_main_window);
}

#if ENABLE_CLOCK_APP
static void update_time()
{
    //Get a tm structure
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    
    //Write the current hours and minutes into a buffer
    static char s_buffer[8];
    strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);

    //Display this time on the text layer
    text_layer_set_text(s_text_layer, s_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
    update_time();
}
#endif //ENABLE_CLOCK_APP

#if ENABLE_GARAGE_DOOR_APP
void config_provider(Window *window)
{
    //single click / repeat-on-hold config:
    window_single_click_subscribe(BUTTON_ID_DOWN, down_single_click_handler);
    window_single_click_subscribe(BUTTON_ID_UP, up_single_click_handler);
    window_single_click_subscribe(BUTTON_ID_SELECT, select_single_click_handler);
}

void down_single_click_handler(ClickRecognizerRef recognizer, void *context)
{
    Window *window = (Window *)context;
    text_layer_set_text(s_text_layer, "Garage Door Status: Close");
}

void up_single_click_handler(ClickRecognizerRef recognizer, void *context)
{
    Window *window = (Window *)context;
    text_layer_set_text(s_text_layer, "Garage Door Status: Open");
}

void select_single_click_handler(ClickRecognizerRef recognizer, void *context)
{
    Window *window = (Window *)context;
    text_layer_set_text(s_text_layer, "Garage Door Status Checking...");
}
#endif //ENABLE_GARAGE_DOOR_APP

static void main_window_load(Window *window)
{
    //Get information about the window
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    //Create the textlayer with specific bounds
    s_text_layer = text_layer_create(
        GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, bounds.size.h));

    //Improve the layout to be more like a watchface
    text_layer_set_background_color(s_text_layer, GColorClear);
    text_layer_set_text_color(s_text_layer, GColorBlack);
    text_layer_set_overflow_mode(s_text_layer, GTextOverflowModeWordWrap);
#if ENABLE_CLOCK_APP
    text_layer_set_text(s_text_layer, "00:00");
    text_layer_set_font(s_text_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
#endif //ENABLE_CLOCK_APP

#if ENABLE_GARAGE_DOOR_APP
    text_layer_set_text(s_text_layer, "Garage Door Status: Close");
    text_layer_set_font(s_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
#endif //ENABLE_GARAGE_DOOR_APP

    text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);

    //Add it as a child layer to the window's root layer
    layer_add_child(window_layer, text_layer_get_layer(s_text_layer));
}

static void main_window_unload(Window *window)
{
    //Destroy TextLayer
    text_layer_destroy(s_text_layer); 
}



