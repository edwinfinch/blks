#include <pebble.h>
	
Window *window;
TextLayer *hour_1, *hour_2, *minute_1, *minute_2;
BitmapLayer *background_layer, *bt_image_layer;
GBitmap *background_image, *bt_image;
Layer *battery_layer;
AppTimer *charge_timer;
bool cancelled = 0;
int battery_percent;
bool invert;
	
static TextLayer* text_layer_init(GRect location)
{
	TextLayer *layer = text_layer_create(location);
	text_layer_set_text_color(layer, GColorWhite);
	text_layer_set_background_color(layer, GColorClear);
	text_layer_set_text_alignment(layer, GTextAlignmentCenter);
	text_layer_set_font(layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_IMPACT_48)));
	return layer;
}

void tick_handler(struct tm *t, TimeUnits units_changed){
	int minute = t->tm_min;
	int hour = t->tm_hour;
	static char min_1_buf[] = "1";
	static char min_2_buf[] = "2";
	static char hour_1_buf[] = "1";
	static char hour_2_buf[] = "2";
	
	int fixmin1 = minute/10;
	int fixmin2 = minute%10;
	int fixhour1, fixhour2;
	if(clock_is_24h_style()){
		fixhour1 = hour/10;
		fixhour2 = hour%10;
	}
	else{
		if(hour > 12){
			hour -= 12;
		}
		fixhour1 = hour/10;
		fixhour2 = hour%10;
	}
	
	snprintf(min_1_buf, sizeof(min_1_buf), "%d", fixmin1);
	snprintf(min_2_buf, sizeof(min_2_buf), "%d", fixmin2);
	snprintf(hour_1_buf, sizeof(hour_1_buf), "%d", fixhour1);
	snprintf(hour_2_buf, sizeof(hour_2_buf), "%d", fixhour2);
	
	text_layer_set_text(minute_1, min_1_buf);
	text_layer_set_text(minute_2, min_2_buf);
	text_layer_set_text(hour_1, hour_1_buf);
	text_layer_set_text(hour_2, hour_2_buf);
}

void battery_proc(Layer *layer, GContext *ctx){
	int height = 158;
	int circle_radius = 4;
	int k, l;
	for(k = 10; k > 0; k--){
		l = (13*k);
		graphics_draw_circle(ctx, GPoint(l, height), circle_radius);
	}
	
	int i, j;
	for(i = battery_percent/10; i > 0; i--){
		j = (i*13);
		graphics_fill_circle(ctx, GPoint(j, height), circle_radius);
	}
}

void charge_invert(void *data){
	invert = !invert;
	if(invert){
		if(battery_percent != 100){
			battery_percent += 10;
		}
		layer_mark_dirty(battery_layer);
	}
	else{
		if(battery_percent != 0){
			battery_percent -= 10;
		}
		layer_mark_dirty(battery_layer);
	}
	charge_timer = app_timer_register(1000, charge_invert, NULL);
}

void battery_handler(BatteryChargeState charge){
	battery_percent = charge.charge_percent;
	layer_mark_dirty(battery_layer);
	
	if(charge.is_charging){
		cancelled = 0;
		app_timer_cancel(charge_timer);
		charge_timer = app_timer_register(1000, charge_invert, NULL);
	}
	else{
		if(!cancelled){
			app_timer_cancel(charge_timer);
			cancelled = 1;
		}
	}
}

void bt_handler(bool connected){
	layer_set_hidden(bitmap_layer_get_layer(bt_image_layer), !connected);
}
	
void window_load(Window *window){
	Layer *window_layer = window_get_root_layer(window);
	
	background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
	bitmap_layer_set_bitmap(background_layer, background_image);
	layer_add_child(window_layer, bitmap_layer_get_layer(background_layer));
	
	bt_image_layer = bitmap_layer_create(GRect(0, -8, 144, 168));
	bitmap_layer_set_bitmap(bt_image_layer, bt_image);
	layer_add_child(window_layer, bitmap_layer_get_layer(bt_image_layer));
	
	hour_1 = text_layer_init(GRect(22, 8, 28, 50));
	layer_add_child(window_layer, text_layer_get_layer(hour_1));
	
	hour_2 = text_layer_init(GRect(97, 8, 28, 50));
	layer_add_child(window_layer, text_layer_get_layer(hour_2));
	
	minute_1 = text_layer_init(GRect(22, 85, 28, 50));
	layer_add_child(window_layer, text_layer_get_layer(minute_1));
	
	minute_2 = text_layer_init(GRect(97, 85, 28, 50));
	layer_add_child(window_layer, text_layer_get_layer(minute_2));
	
	battery_layer = layer_create(GRect(0, 0, 144, 168));
	layer_set_update_proc(battery_layer, battery_proc);
	layer_add_child(window_layer, battery_layer);
	
	struct tm *t;
  	time_t temp;        
  	temp = time(NULL);        
  	t = localtime(&temp);
	
	tick_handler(t, MINUTE_UNIT);
	
	BatteryChargeState bat = battery_state_service_peek();
	battery_handler(bat);
	
	bool hello = bluetooth_connection_service_peek();
	bt_handler(hello);
}

void window_unload(Window *window){
	text_layer_destroy(hour_1);
	text_layer_destroy(hour_2);
	text_layer_destroy(minute_1);
	text_layer_destroy(minute_2);
	bitmap_layer_destroy(bt_image_layer);
	bitmap_layer_destroy(background_layer);
}
	
void init(){
	window = window_create();
	window_set_window_handlers(window, (WindowHandlers){
		.load = window_load,
		.unload = window_unload,
	});
	tick_timer_service_subscribe(MINUTE_UNIT, &tick_handler);
	battery_state_service_subscribe(battery_handler);
	bluetooth_connection_service_subscribe(bt_handler);
	
	background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BASE);
	bt_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BT_ICON);
	window_stack_push(window, true);
}

void deinit(){
	tick_timer_service_unsubscribe();
	battery_state_service_unsubscribe();
	bluetooth_connection_service_unsubscribe();
	gbitmap_destroy(background_image);
	gbitmap_destroy(bt_image);
}

int main(){
	init();
	app_event_loop();
	deinit();
}