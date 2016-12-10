#include <allegro5\allegro.h>
#include <allegro5\allegro_image.h>
#include <allegro5\allegro_primitives.h>
#include <string>
#include <fstream>
#include <cstdio>

using namespace std;
enum KEYS { UP, DOWN, LEFT, RIGHT };

typedef struct Tile
{
	int width = 50;
	int height = 50;
	int x;
	int y;
	int type;

} Tile;

typedef struct Map
{
	int width;
	int height;
	
	Tile **tiles;

} Map;

Map Load_Map(string map_path)
{
	Map map;

	ifstream f;
	f.open("Maps\\map1.txt");
	f >> map.width;
	f >> map.height;

	int h = 0;
	int w = 0;

	map.tiles = new Tile*[map.height];

	for (int h = 0; h < map.height; h++)
	{
		map.tiles[h] = new Tile[map.width];

		for (int w = 0; w < map.width; w++)
		{
			map.tiles[h][w].x = w;
			map.tiles[h][w].y = h;
			f >> map.tiles[h][w].type;
		}
	}

	return map;
}

void Draw_Map(Map map, ALLEGRO_BITMAP *tile_bitmaps[10], int map_x, int map_y) {
	int ih;
	int iw;
	for (ih = 0; ih < map.height; ih++)
	{
		for (iw = 0; iw < map.width; iw++)
		{
			switch(map.tiles[ih][iw].type){
			case 11:
				al_draw_bitmap(tile_bitmaps[1], map_x + iw*map.tiles[ih][iw].width, map_y + ih*map.tiles[ih][iw].height, 0);
				break;
			case 21:
				al_draw_bitmap(tile_bitmaps[2], map_x + iw*map.tiles[ih][iw].width, map_y + ih*map.tiles[ih][iw].height, 0);
				break;
			default:
				al_draw_bitmap(tile_bitmaps[0], map_x + iw*map.tiles[ih][iw].width, map_y + ih*map.tiles[ih][iw].height, 0);
				break;
			}
		}
	}
}
		
typedef struct Window 
{
	int width;
	int height;
	int x;
	int y;

} Window;

typedef struct Interface 
{
	int width = 1600;
	int height = 900;

	int w = 23;
	int h = 16;

	ALLEGRO_BITMAP *bitmap = NULL;

	Window main_map;
	
	Window mini_map;

} Interface;

Interface Load_Interface(string interface_path)
{
	Interface interface;

	interface.main_map.width = 1150;
	interface.main_map.height = 800;
	interface.main_map.x = 40;
	interface.main_map.y = 50;

	interface.mini_map.width = 330;
	interface.mini_map.height = 220;
	interface.mini_map.x = 1230;
	interface.mini_map.y = 50;

	return interface;
}

void Draw_Interface(ALLEGRO_BITMAP *infc_bitmap)
{
	al_draw_bitmap(infc_bitmap, 0, 0, 0);
}

void Draw_Minimap(Map map, Interface interface, int mini_map_x, int mini_map_y) {
	int ih;
	int iw;
	for (ih = 0; ih < map.height; ih++)
	{
		for (iw = 0; iw < map.width; iw++)
		{
			switch (map.tiles[ih][iw].type) {
			case 11:
				al_draw_filled_rectangle(interface.mini_map.x + iw * 5, interface.mini_map.y + ih * 5,
										interface.mini_map.x + iw * 5 + 5, interface.mini_map.y + ih * 5 + 5,
										al_map_rgb(0, 255, 0));
				break;
			case 21:
				al_draw_filled_rectangle(interface.mini_map.x + iw * 5, interface.mini_map.y + ih * 5,
										interface.mini_map.x + iw * 5 + 5, interface.mini_map.y + ih * 5 + 5,
										al_map_rgb(141, 54, 54));
				break;
			default:
				al_draw_filled_rectangle(interface.mini_map.x + iw * 5, interface.mini_map.y + ih * 5,
										interface.mini_map.x + iw * 5 + 5, interface.mini_map.y + ih * 5 + 5,
										al_map_rgb(0, 0, 0));
				break;
			}
		}
	}

	al_draw_rectangle(mini_map_x, mini_map_y, mini_map_x + interface.w * 5, mini_map_y + interface.h * 5, al_map_rgb(255, 255, 255), 2);



}

int main(void)
{
	bool keys[4] = { false, false, false, false };
	bool done = false;

	al_init();
	if (!al_init())																				//initialize Allegro
		return -1;

	int FPS = 60;

	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_TIMER *timer = NULL;

	al_init_image_addon();
	al_init_primitives_addon();
	al_install_keyboard();
	al_install_mouse();

	Map map = Load_Map("Maps\\map1.txt");
	Interface interface = Load_Interface("Bitmaps\\Interface\\Interface.bmp");

	display = al_create_display(interface.width, interface.height); 							//create our display object
	if (!display)																				//test display object
		return -1;

	interface.bitmap = al_load_bitmap("Bitmaps\\Interface\\Interface.bmp");
	if (!interface.bitmap)
		return -2;
	al_convert_mask_to_alpha(interface.bitmap, al_map_rgb(255, 0, 255));

	ALLEGRO_BITMAP *tile_bitmaps[10];
	tile_bitmaps[0] = al_load_bitmap("Bitmaps/Tiles/Empty.bmp");
	tile_bitmaps[1] = al_load_bitmap("Bitmaps/Tiles/Grass.bmp");
	tile_bitmaps[2] = al_load_bitmap("Bitmaps/Tiles/Road.bmp");

	event_queue = al_create_event_queue();
	timer = al_create_timer(1.0 / FPS);
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_start_timer(timer);

	int map_x = interface.main_map.x;
	int map_y = interface.main_map.y;

	int mini_map_x = interface.mini_map.x;
	int mini_map_y = interface.mini_map.y;

	while (!done) {
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			switch (ev.keyboard.keycode)
			{
			case ALLEGRO_KEY_UP:
				keys[UP] = true;
				break;
			case ALLEGRO_KEY_DOWN:
				keys[DOWN] = true;
				break;
			case ALLEGRO_KEY_LEFT:
				keys[LEFT] = true;
				break;
			case ALLEGRO_KEY_RIGHT:
				keys[RIGHT] = true;
				break;
			}
		}
		else if (ev.type == ALLEGRO_EVENT_KEY_UP)
		{
			switch (ev.keyboard.keycode)
			{
			case ALLEGRO_KEY_UP:
				keys[UP] = false;
				break;
			case ALLEGRO_KEY_DOWN:
				keys[DOWN] = false;
				break;
			case ALLEGRO_KEY_LEFT:
				keys[LEFT] = false;
				break;
			case ALLEGRO_KEY_RIGHT:
				keys[RIGHT] = false;
				break;
			case ALLEGRO_KEY_ESCAPE:
				done = true;
				break;
			}
		}
		else if (ev.type == ALLEGRO_EVENT_TIMER) {
			if (map_y < interface.main_map.y) {
				map_y += keys[UP] * 10;
				mini_map_y -= keys[UP];
			}
			if (map_y > -((map.height * map.tiles[0][0].height) - (interface.main_map.y + interface.main_map.height)))
			{
				map_y -= keys[DOWN] * 10;
				mini_map_y += keys[DOWN];
			}
			if (map_x < interface.main_map.x) {
				map_x += keys[LEFT] * 10;
				mini_map_x -= keys[LEFT];
			}
			if (map_x > -((map.width * map.tiles[0][0].width) - (interface.main_map.x + interface.main_map.width))) 
			{
				map_x -= keys[RIGHT] * 10;
				mini_map_x += keys[RIGHT];
			}
		}

		Draw_Map(map, tile_bitmaps, map_x, map_y);
		Draw_Interface(interface.bitmap);
		Draw_Minimap(map, interface, mini_map_x, mini_map_y);

		al_flip_display();
		al_clear_to_color(al_map_rgb(0, 0, 0));
	}

	al_destroy_bitmap(interface.bitmap);
	al_destroy_event_queue(event_queue);
	al_destroy_display(display);

	return 0;
}
