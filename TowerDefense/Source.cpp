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

void Load_Tile_Bitmaps(ALLEGRO_BITMAP *tile_bitmaps[10]) {

	tile_bitmaps[0] = al_load_bitmap("Bitmaps/Tiles/Empty.bmp");
	tile_bitmaps[1] = al_load_bitmap("Bitmaps/Tiles/Grass.bmp");
	tile_bitmaps[2] = al_load_bitmap("Bitmaps/Tiles/Road.bmp");

}

typedef struct Map
{
	int width;
	int height;
	
	Tile **tiles;

	int ch_x;
	int ch_y;

} Map;

Map Load_Map(string map_path)
{
	Map map;

	ifstream f;
	f.open("Maps\\map2.txt");
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
			if (map.tiles[h][w].type == 99) {
				map.ch_x = w;
				map.ch_y = h;
			}
		}
	}

	return map;
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

	Window status;

	Window actions;

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

	interface.status.width = 330;
	interface.status.height = 125;
	interface.status.x = 1230;
	interface.status.y = 290;

	interface.actions.width = 330;
	interface.actions.height = 400;
	interface.actions.x = 1230;
	interface.actions.y = 450;

	return interface;
}

void Draw_Interface(Interface interface)
{
	al_draw_bitmap(interface.bitmap, 0, 0, 0);
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

			case 99:
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

void Draw_Map(Map map, ALLEGRO_BITMAP *tile_bitmaps[10], int map_x, int map_y) {
	int ih;
	int iw;
	for (ih = 0; ih < map.height; ih++)
	{
		for (iw = 0; iw < map.width; iw++)
		{
			switch (map.tiles[ih][iw].type) {
			case 11:
				al_draw_bitmap(tile_bitmaps[1], map_x + iw*map.tiles[ih][iw].width, map_y + ih*map.tiles[ih][iw].height, 0);
				break;
			case 21:
				al_draw_bitmap(tile_bitmaps[2], map_x + iw*map.tiles[ih][iw].width, map_y + ih*map.tiles[ih][iw].height, 0);
				break;

			case 99:
				al_draw_bitmap(tile_bitmaps[2], map_x + iw*map.tiles[ih][iw].width, map_y + ih*map.tiles[ih][iw].height, 0);
				break;
			default:
				al_draw_bitmap(tile_bitmaps[0], map_x + iw*map.tiles[ih][iw].width, map_y + ih*map.tiles[ih][iw].height, 0);
				break;
			}
		}
	}
}


typedef struct Cursor
{
	int x;
	int y;

	bool buttons[2] = { false, false };

	ALLEGRO_BITMAP *bitmapIdle;
	ALLEGRO_BITMAP *bitmapActive;

} Cursor;

Cursor Load_Cursor(Interface interface)
{
	Cursor cursor;

	cursor.x = interface.main_map.width / 2;
	cursor.y = interface.main_map.height / 2;

	return cursor;
}

void Draw_Cursor(Cursor cursor)
{
	if (cursor.buttons[0] == true || cursor.buttons[1] == true)
	{
		al_draw_bitmap(cursor.bitmapActive, cursor.x, cursor.y, 0);
	}
	else 
	{
		al_draw_bitmap(cursor.bitmapIdle, cursor.x, cursor.y, 0);
	}
}

bool Cursor_On_MainMap(Cursor cursor, Interface interface) 
{
	if ((cursor.x >= interface.main_map.x && cursor.x <=interface.main_map.x + interface.main_map.width) &&
		(cursor.y >= interface.main_map.y && cursor.y <= interface.main_map.y + interface.main_map.height))
		return true;
	return false;
}


typedef struct Panel {

} Panel;

typedef struct Goblin
{
	ALLEGRO_BITMAP *bitmap;

	int pos_x;
	int pos_y;

	int width;
	int height;

	Panel panel;

	bool status_active = false;

} Goblin;

typedef struct Rider
{
	ALLEGRO_BITMAP *bitmap;

	int pos_x;
	int pos_y;

	int width;
	int height;

	int health_current;
	int health_max;

	Panel panel;

	bool status_active = false;

} Rider;

typedef struct Bunker
{
	ALLEGRO_BITMAP *bitmap;

	int pos_x;
	int pos_y;

	int width;
	int height;

	int range;

	Panel panel;

	bool status_active = false;

} Bunker;

typedef struct Cityhall 
{
	ALLEGRO_BITMAP *bitmap;
	ALLEGRO_BITMAP *bitmap_active;

	int pos_x ;
	int pos_y ;

	int width = 3;
	int height = 3;

	int health_current ;
	int health_max = 1000;

	Panel panel ;

	bool status_active;

} CityHall;

typedef struct Lab 
{
	ALLEGRO_BITMAP *bitmap;

	int pos_x;
	int pos_y;

	int width;
	int height;

	Panel panel;

} Lab;


Cityhall Load_Cityhall(Map map)
{
	Cityhall cityhall;
	
	cityhall.health_current = cityhall.health_max;

	cityhall.pos_x = map.ch_x * map.tiles[0][0].width;
	cityhall.pos_y = map.ch_y * map.tiles[0][0].width;

	cityhall.width = cityhall.width * map.tiles[0][0].width;
	cityhall.height = cityhall.height * map.tiles[0][0].height;

	cityhall.status_active = 0;

	return cityhall;
}

void Draw_Cityhall(Cityhall cityhall, Map map, int map_x, int map_y)
{
	if (cityhall.status_active == 1)
	{
		al_draw_bitmap(cityhall.bitmap_active, cityhall.pos_x + map_x, cityhall.pos_y + map_y, 0);
	}
		
	al_draw_bitmap(cityhall.bitmap, cityhall.pos_x + map_x, cityhall.pos_y + map_y, 0);
}

void Set_Cityhall_Active(Cityhall *cityhall) 
{
	printf("active \n");
	cityhall->status_active = 1;
}

void Set_Cityhall_InActive(Cityhall *cityhall)
{
	printf("inactive  \n");
	cityhall->status_active = 0;
}

int main(void)
{

	// variables
	bool keys[4] = { false, false, false, false };
	bool done = false;
	bool redraw = true;
	int FPS = 60;

	//allegro variables
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_TIMER *timer = NULL;

	//allegro inits
	al_init();
	if (!al_init())
		return -1;
	al_init_image_addon();
	al_init_primitives_addon();
	al_install_keyboard();
	al_install_mouse();

	// map
	Map map = Load_Map("Maps\\map1.txt");

	// Display & interface
	Interface interface = Load_Interface("Bitmaps\\Interface\\Interface.bmp");
	display = al_create_display(interface.width, interface.height);
	if (!display)
		return -2;
	interface.bitmap = al_load_bitmap("Bitmaps\\Interface\\Interface.bmp");
	if (!interface.bitmap)
		return -3;
	al_convert_mask_to_alpha(interface.bitmap, al_map_rgb(255, 0, 255));

	// Cursor
	Cursor cursor = Load_Cursor(interface);
	cursor.bitmapIdle = al_load_bitmap("Bitmaps\\Interface\\Cursor.bmp");
	if (!cursor.bitmapIdle)
		return -4;
	al_convert_mask_to_alpha(cursor.bitmapIdle, al_map_rgb(255, 0, 255));
	cursor.bitmapActive = al_load_bitmap("Bitmaps\\Interface\\CursorActive.bmp");
	if (!cursor.bitmapActive)
		return -4;
	al_convert_mask_to_alpha(cursor.bitmapActive, al_map_rgb(255, 0, 255));
	al_hide_mouse_cursor(display);

	// Tile bitmaps
	ALLEGRO_BITMAP *tile_bitmaps[10];
	Load_Tile_Bitmaps(tile_bitmaps);

	// event handling
	event_queue = al_create_event_queue();
	timer = al_create_timer(1.0 / FPS);
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_mouse_event_source());
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_start_timer(timer);

	//position variables
	int map_x = interface.main_map.x;
	int map_y = interface.main_map.y;
	int mini_map_x = interface.mini_map.x;
	int mini_map_y = interface.mini_map.y;

	// Game objects
	// Cityhall
	Cityhall cityhall = Load_Cityhall(map);
	cityhall.bitmap = al_load_bitmap("Bitmaps\\GameObjects\\Cityhall.bmp");
	if (!cityhall.bitmap)
		return -5;
	cityhall.bitmap_active = al_load_bitmap("Bitmaps\\GameObjects\\Halo_Big.bmp");
	if (!cityhall.bitmap_active)
		return -5;
	al_convert_mask_to_alpha(cityhall.bitmap, al_map_rgb(255, 0, 255));
	al_convert_mask_to_alpha(cityhall.bitmap_active, al_map_rgb(255, 0, 255));


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
		else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			done = true;
		}
		else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES)
		{
			cursor.x = ev.mouse.x;
			cursor.y = ev.mouse.y;
			redraw = true;
		}
		else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			switch (ev.mouse.button){
			case 1:
				cursor.buttons[0] = true;
				Set_Cityhall_Active(&cityhall);
				break;
			case 2:
				cursor.buttons[1] = true;
				Set_Cityhall_InActive(&cityhall);
				break;
			}
		}
		else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
			switch (ev.mouse.button) {
			case 1:
				cursor.buttons[0] = false;
				break;
			case 2:
				cursor.buttons[1] = false;
				break;
			}
		}
		else if (ev.type == ALLEGRO_EVENT_TIMER)
		{
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

			if (Cursor_On_MainMap(cursor, interface)==true) {
				printf("%i ", 1);
			}
			else
			{
				printf("%i ", 0);
			}


			redraw = true;
		}

		if (redraw && al_is_event_queue_empty(event_queue)) {
			redraw = false;

			Draw_Map(map, tile_bitmaps, map_x, map_y);
			Draw_Cityhall(cityhall, map, map_x, map_y);


			Draw_Interface(interface);
			Draw_Minimap(map, interface, mini_map_x, mini_map_y);
			Draw_Cursor(cursor);

			al_flip_display();
			al_clear_to_color(al_map_rgb(0, 0, 0));
		}
	}

	al_destroy_bitmap(interface.bitmap);
	al_destroy_event_queue(event_queue);
	al_destroy_display(display);

	return 0;
}
