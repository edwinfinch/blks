#include <pebble.h>
#include "elements.h"
void animation_callback(void *data);
	
static TextLayer* text_layer_init(GRect location)
{
	TextLayer *layer = text_layer_create(location);
	text_layer_set_text_color(layer, GColorBlack);
	text_layer_set_background_color(layer, GColorClear);
	text_layer_set_text_alignment(layer, GTextAlignmentCenter);
	text_layer_set_font(layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_IMPACT_48)));
	return layer;
}

void stopped(Animation *anim, bool finished, void *context)
{
    property_animation_destroy((PropertyAnimation*) anim);
}
 
void animate_layer(Layer *layer, GRect *start, GRect *finish, int duration, int delay)
{
    PropertyAnimation *anim = property_animation_create_layer_frame(layer, start, finish);
     
    animation_set_duration((Animation*) anim, duration);
    animation_set_delay((Animation*) anim, delay);
     
    AnimationHandlers handlers = {
        .stopped = (AnimationStoppedHandler) stopped
    };
    animation_set_handlers((Animation*) anim, handlers, NULL);
     
    animation_schedule((Animation*) anim);
}

void animate(){
	animate_layer(text_layer_get_layer(cov_1), &initial_1, &final_1, 700, 300);
	animate_layer(text_layer_get_layer(cov_2), &initial_2, &final_2, 700, 300);
	animate_layer(text_layer_get_layer(cov_3), &initial_3, &final_3, 700, 300);
	animate_layer(text_layer_get_layer(cov_4), &initial_4, &final_4, 700, 300);
		
	animate_layer(text_layer_get_layer(cov_1), &final_1, &initial_1, 700, 1010);
	animate_layer(text_layer_get_layer(cov_2), &final_2, &initial_2, 700, 1010);
	animate_layer(text_layer_get_layer(cov_3), &final_3, &initial_3, 700, 1010);
	animate_layer(text_layer_get_layer(cov_4), &final_4, &initial_4, 700, 1010);
}

void tick_handler(struct tm *t, TimeUnits units_changed){
	int minute = t->tm_min;
	int hour = t->tm_hour;
	int seconds = t->tm_sec;
	static char min_1_buf[] = "1";
	static char min_2_buf[] = "2";
	static char hour_1_buf[] = "1";
	static char hour_2_buf[] = "2";
	static char date_buf[] = "Thu.";
	
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
	
	strftime(date_buf, sizeof(date_buf), "%a", t);
	text_layer_set_text(date_layer, date_buf);
	
	if(seconds == 59){
		animate();
	}
}

void battery_proc(Layer *layer, GContext *ctx){
	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_context_set_stroke_color(ctx, GColorWhite);
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

void tap(AccelAxisType axis, int32_t direction){
	if(showing_date){
		layer_set_hidden(bitmap_layer_get_layer(bt_image_layer), false);
		layer_set_hidden(text_layer_get_layer(date_layer), true);
	}
	else{
		layer_set_hidden(bitmap_layer_get_layer(bt_image_layer), true);
		layer_set_hidden(text_layer_get_layer(date_layer), false);
	}
	showing_date = !showing_date;
}
	
void circle_proc(Layer *layer, GContext *ctx){
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_fill_circle(ctx, GPoint(72, 76), 19);
}

void window_load(Window *window){
	Layer *window_layer = window_get_root_layer(window);
	
	final_1 = GRect(19, 14, COVER_W, COVER_L);
	final_2 = GRect(95, 14, COVER_W, COVER_L);
	final_3 = GRect(19, 90, COVER_W, COVER_L);
	final_4 = GRect(95, 90, COVER_W, COVER_L);

	initial_1 = GRect(19, 14, 1, 1);
	initial_2 = GRect(95, 14, 1, 1);
	initial_3 = GRect(19, 90, 1, 1);
	initial_4 = GRect(95, 90, 1, 1);
	
	background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
	bitmap_layer_set_bitmap(background_layer, background_image);
	layer_add_child(window_layer, bitmap_layer_get_layer(background_layer));
	
	circle_layer = layer_create(GRect(0, 0, 144, 168));
	layer_set_update_proc(circle_layer, circle_proc);
	layer_add_child(window_layer, circle_layer);
	
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
	
	cov_1 = text_layer_init(GRect(19, 14, 1, 1));
	text_layer_set_background_color(cov_1, GColorWhite);
	layer_add_child(window_layer, text_layer_get_layer(cov_1));
	
	cov_2 = text_layer_init(GRect(95, 14, 1, 1));
	text_layer_set_background_color(cov_2, GColorWhite);
	layer_add_child(window_layer, text_layer_get_layer(cov_2));
	
	cov_3 = text_layer_init(GRect(19, 90, 1, 1));
	text_layer_set_background_color(cov_3, GColorWhite);
	layer_add_child(window_layer, text_layer_get_layer(cov_3));
	
	cov_4 = text_layer_init(GRect(95, 90, 1, 1));
	text_layer_set_background_color(cov_4, GColorWhite);
	layer_add_child(window_layer, text_layer_get_layer(cov_4));
	
	date_layer = text_layer_init(GRect(0, 65, 144, 168));
	text_layer_set_font(date_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_IMPACT_18)));
	text_layer_set_text_color(date_layer, GColorWhite);
	layer_add_child(window_layer, text_layer_get_layer(date_layer));
	
	layer_set_hidden(text_layer_get_layer(date_layer), true);
	
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
	layer_destroy(battery_layer);
}
	
void init(){
	window = window_create();
	window_set_window_handlers(window, (WindowHandlers){
		.load = window_load,
		.unload = window_unload,
	});
	tick_timer_service_subscribe(SECOND_UNIT, &tick_handler);
	battery_state_service_subscribe(battery_handler);
	bluetooth_connection_service_subscribe(bt_handler);
	accel_tap_service_subscribe(&tap);
	
	background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BASE);
	bt_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BT_ICON);
	window_stack_push(window, true);
}

void deinit(){
	tick_timer_service_unsubscribe();
	battery_state_service_unsubscribe();
	bluetooth_connection_service_unsubscribe();
	accel_tap_service_unsubscribe();
	gbitmap_destroy(background_image);
	gbitmap_destroy(bt_image);
}

int main(){
	init();
	app_event_loop();
	deinit();
}