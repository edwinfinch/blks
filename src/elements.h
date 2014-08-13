Window *window;

TextLayer *hour_1, *hour_2, *minute_1, *minute_2, *date_layer;

BitmapLayer *background_layer, *bt_image_layer;
GBitmap *background_image, *bt_image;

//Battery stuff
Layer *battery_layer;
AppTimer *charge_timer;
bool cancelled = 0;
int battery_percent;
bool invert;
//End battery stuff

InverterLayer *theme;

bool showing_date = 0;

//Animation stuff.
Layer *circle_layer;
int animation_runs = 0;
int public_radius = 0;
bool second_stage = 0;
AppTimer *animation_timer;