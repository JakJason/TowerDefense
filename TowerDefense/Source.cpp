#include <allegro5\allegro.h>
#include <allegro5\allegro_image.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
#include <string>
#include <fstream>
#include <cstdio>
#include <list>
#include <functional>
#include <stdbool.h>

using namespace std;
enum KEYS { UP, DOWN, LEFT, RIGHT };

typedef void(*Action)();
////////////////Structs////////////////////////////
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

	int ch_x;
	int ch_y;

	int s_x;
	int s_y;

} Map;
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
typedef struct Cursor
{
	int x;
	int y;

	int t_x;
	int t_y;

	int cursor_task = 0; // 1 - build bunker, 2 - build lab

	bool buttons[2] = { false, false };

	ALLEGRO_BITMAP *bitmapIdle;
	ALLEGRO_BITMAP *bitmapActive;

} Cursor;
typedef struct Button{

	ALLEGRO_BITMAP * bitmap= NULL;
	int x;
	int y;

} Button;
typedef struct Panel {

	int health_current = NULL;
	int health_max = NULL;
	
	string name = " ";

	// function 00
	Button button00;

	// function 01
	Button button01;

	// function 02
	Button button02;

	// function 10
	Button button10;

	// function 11
	Button button11;

	// function 12
	Button button12;

	// function 20
	Button button20;

	// function 21
	Button button21;

	// function 22
	Button button22;

	ALLEGRO_BITMAP *icon = NULL;

} Panel;
typedef struct Goblin
{
	ALLEGRO_BITMAP *bitmap;
	ALLEGRO_BITMAP *bitmap_active;

	int tile_pos_x;
	int tile_pos_y;

	float pos_x;
	float pos_y;

	int width = 1;
	int height = 1;

	float speed = 1.25;

	Panel panel;

	int busy = 0; // 1 - build bunker 2 - build lab

	bool status_active = false;

	list<Tile*> path;
} Goblin;
typedef struct Rider
{
	ALLEGRO_BITMAP *bitmap;
	ALLEGRO_BITMAP *bitmap_active;

	int tile_pos_x;
	int tile_pos_y;

	float pos_x;
	float pos_y;

	int width = 1;
	int height = 1;

	float speed = 1;

	Panel panel;

	bool status_active = false;

	list<Tile*> path;

} Rider;
typedef struct Bunker
{
	ALLEGRO_BITMAP *bitmap;
	ALLEGRO_BITMAP *bitmap_active;

	int pos_x;
	int pos_y;

	int width = 2;
	int height = 2;

	float range = 4;
	int cooldown = 90;
	int max_cooldown = 90;

	Rider *target = NULL;
	Panel panel;

	bool status_active = false;

} Bunker;
typedef struct Cityhall 
{
	ALLEGRO_BITMAP *bitmap;
	ALLEGRO_BITMAP *bitmap_active;

	int pos_x ;
	int pos_y ;

	int entrance_x;
	int entrance_y;

	int width = 3;
	int height = 3;

	Panel panel ;

	bool status_active;

} CityHall;
typedef struct Lab 
{
	ALLEGRO_BITMAP *bitmap;
	ALLEGRO_BITMAP *bitmap_active;

	int pos_x;
	int pos_y;

	int width = 2;
	int height = 2;

	Panel panel;

	bool status_active = false;

} Lab;
typedef struct Clock {
	int t;
	int s;
	int m;
	int h;
}Clock;
typedef struct BuildUp {
	ALLEGRO_BITMAP *bitmap;
	ALLEGRO_BITMAP *bitmap_active;

	int pos_x;
	int pos_y;

	int width = 2;
	int height = 2;

	Panel panel;

	int cost = 200;
	int partial = 0;
	int full = 1800;

	int type = 0; // 1- bunker, 2- lab

	bool status_active = false;
} BuildUp;
////////////////////////////////////////////////////

////////////////Load///////////////////////////////
Button Load_Button(ALLEGRO_BITMAP* icon, int x , int y) {
	Button button;
	
	button.bitmap = icon;
	button.x = x;
	button.y = y;

	return button;
}
Panel Load_Panel(ALLEGRO_BITMAP *icon1, ALLEGRO_BITMAP *icon2, ALLEGRO_BITMAP *icon3,
	ALLEGRO_BITMAP *icon4, ALLEGRO_BITMAP *icon5, ALLEGRO_BITMAP *icon6, 
	ALLEGRO_BITMAP *icon7, ALLEGRO_BITMAP *icon8, ALLEGRO_BITMAP *icon9) {
	Panel panel;
	if (icon1 != NULL) {
		panel.button00 = Load_Button(icon1, 1240, 460);
	}

	if (icon2 != NULL) {
		panel.button01 = Load_Button(icon2, 1345, 460);
	}

	if (icon3 != NULL) {
		panel.button02 = Load_Button(icon3, 1450, 460);
	}

	if (icon4) {
		panel.button10 = Load_Button(icon4, 1240, 565);
	}

	if (icon5) {
		panel.button11 = Load_Button(icon5, 1345, 565);
	}

	if (icon6) {
		panel.button12 = Load_Button(icon6, 1450, 565);
	}

	if (icon7) {
		panel.button20 = Load_Button(icon7, 1240, 670);
	}

	if (icon8) {
		panel.button21 = Load_Button(icon8, 1345, 670);
	}

	if (icon9) {
		panel.button22 = Load_Button(icon9, 1450, 670);
	}

	return panel;
}
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

	for (h = 0; h < map.height; h++)
	{
		map.tiles[h] = new Tile[map.width];

		for (w = 0; w < map.width; w++)
		{
			map.tiles[h][w].x = w;
			map.tiles[h][w].y = h;
			f >> map.tiles[h][w].type;
			if (map.tiles[h][w].type == 99) {
				map.ch_x = w;
				map.ch_y = h;
			}
			else if (map.tiles[h][w].type == 91) {
				map.s_x = w;
				map.s_y = h;
			}
		}
	}

	return map;
}
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
Cursor Load_Cursor(Interface interface)
{
	Cursor cursor;

	cursor.bitmapIdle = al_load_bitmap("Bitmaps\\Interface\\Cursor\\Cursor.bmp");
	al_convert_mask_to_alpha(cursor.bitmapIdle, al_map_rgb(255, 0, 255));

	cursor.bitmapActive = al_load_bitmap("Bitmaps\\Interface\\Cursor\\CursorActive.bmp");
	al_convert_mask_to_alpha(cursor.bitmapActive, al_map_rgb(255, 0, 255));

	cursor.x = interface.main_map.width / 2;
	cursor.y = interface.main_map.height / 2;

	return cursor;
}
Cityhall Load_Cityhall(Map map)
{
	Cityhall cityhall;

	cityhall.bitmap = al_load_bitmap("Bitmaps\\GameObjects\\CityHall\\Cityhall.bmp");
	al_convert_mask_to_alpha(cityhall.bitmap, al_map_rgb(255, 0, 255));

	cityhall.bitmap_active = al_load_bitmap("Bitmaps\\GameObjects\\Halo\\Halo_Big.bmp");
	al_convert_mask_to_alpha(cityhall.bitmap_active, al_map_rgb(255, 0, 255));

	cityhall.pos_x = map.ch_x * map.tiles[0][0].width;
	cityhall.pos_y = map.ch_y * map.tiles[0][0].width;

	cityhall.width = cityhall.width * map.tiles[0][0].width;
	cityhall.height = cityhall.height * map.tiles[0][0].height;

	cityhall.entrance_x = map.ch_x + 1;
	cityhall.entrance_y = map.ch_y + 3;

	cityhall.status_active = 0;

	map.tiles[map.ch_y][map.ch_x].type = 99;
	map.tiles[map.ch_y + 1][map.ch_x].type = 99;
	map.tiles[map.ch_y + 2][map.ch_x].type = 99;
	map.tiles[map.ch_y][map.ch_x + 1].type = 99;
	map.tiles[map.ch_y + 1][map.ch_x + 1].type = 99;
	map.tiles[map.ch_y + 2][map.ch_x + 1].type = 99;
	map.tiles[map.ch_y][map.ch_x + 2].type = 99;
	map.tiles[map.ch_y + 1][map.ch_x + 2].type = 99;
	map.tiles[map.ch_y + 2][map.ch_x + 2].type = 99;

	ALLEGRO_BITMAP *bitmap00 = al_load_bitmap("Bitmaps/Interface/Icons/Icon_2.bmp");
	ALLEGRO_BITMAP *bitmap01 = al_load_bitmap("Bitmaps/Interface/Icons/Icon_3.bmp");
	ALLEGRO_BITMAP *bitmap02 = al_load_bitmap("Bitmaps/Interface/Icons/Icon_4.bmp");

	cityhall.panel = Load_Panel(bitmap00, bitmap01, bitmap02, NULL, NULL, NULL, NULL, NULL, NULL);

	cityhall.panel.health_max = 1000;
	cityhall.panel.health_current = cityhall.panel.health_max;
	cityhall.panel.name = "City Hall";

	cityhall.panel.icon = al_load_bitmap("Bitmaps\\GameObjects\\CityHall\\Cityhall_Icon.bmp");
	al_convert_mask_to_alpha(cityhall.panel.icon, al_map_rgb(255, 0, 255));

	return cityhall;
}
Goblin Load_Goblin(Map map, int tile_x, int tile_y) {
	Goblin goblin;

	goblin.bitmap = al_load_bitmap("Bitmaps\\GameObjects\\Goblin\\Goblin.bmp");
	al_convert_mask_to_alpha(goblin.bitmap, al_map_rgb(255, 0, 255));

	goblin.bitmap_active = al_load_bitmap("Bitmaps\\GameObjects\\Halo\\Halo_Small.bmp");
	al_convert_mask_to_alpha(goblin.bitmap_active, al_map_rgb(255, 0, 255));

	goblin.tile_pos_x = tile_x;
	goblin.tile_pos_y = tile_y;

	goblin.pos_x = (tile_x) * (map.tiles[0][0].width);
	goblin.pos_y = (tile_y) * (map.tiles[0][0].height);

	goblin.width = goblin.width *  map.tiles[0][0].width;
	goblin.height = goblin.height * map.tiles[0][0].height;

	ALLEGRO_BITMAP *bitmap00 = al_load_bitmap("Bitmaps/Interface/Icons/Bunker_Button.bmp");
	ALLEGRO_BITMAP *bitmap01 = al_load_bitmap("Bitmaps/Interface/Icons/Lab_Button.bmp");

	goblin.panel = Load_Panel(bitmap00, bitmap01, 0, 0, 0, 0, 0, 0, 0);
	goblin.panel.name = "Goblin";
	goblin.panel.icon = al_load_bitmap("Bitmaps\\GameObjects\\Goblin\\Goblin_Icon.bmp");
	al_convert_mask_to_alpha(goblin.panel.icon, al_map_rgb(255, 0, 255));

	return goblin;
}
Rider Load_Rider(Map map, int tile_x, int tile_y, int health, float speed) {
	Rider rider;

	rider.bitmap = al_load_bitmap("Bitmaps\\GameObjects\\Rider\\Rider.bmp");
	al_convert_mask_to_alpha(rider.bitmap, al_map_rgb(255, 0, 255));

	rider.bitmap_active = al_load_bitmap("Bitmaps\\GameObjects\\Halo\\Halo_Small.bmp");
	al_convert_mask_to_alpha(rider.bitmap_active, al_map_rgb(255, 0, 255));

	rider.tile_pos_x = tile_x;
	rider.tile_pos_y = tile_y;

	rider.pos_x = (tile_x) * (map.tiles[0][0].width);
	rider.pos_y = (tile_y) * (map.tiles[0][0].height);

	rider.width = rider.width *  map.tiles[0][0].width;
	rider.height = rider.height * map.tiles[0][0].height;

	rider.speed = speed;

	rider.panel.health_max = health;
	rider.panel.health_current = health;

	rider.panel.name = "Rider";
	rider.panel.icon = al_load_bitmap("Bitmaps\\GameObjects\\Rider\\Rider_Icon.bmp");
	al_convert_mask_to_alpha(rider.panel.icon, al_map_rgb(255, 0, 255));

	return rider;
}
void Load_Tile_Bitmaps(ALLEGRO_BITMAP *tile_bitmaps[10]) {

	tile_bitmaps[0] = al_load_bitmap("Bitmaps/Tiles/Empty.bmp");
	tile_bitmaps[1] = al_load_bitmap("Bitmaps/Tiles/Grass.bmp");
	tile_bitmaps[2] = al_load_bitmap("Bitmaps/Tiles/Road.bmp");

}
Clock Load_Clock() {
	Clock clock;

	clock.t = 0;
	clock.s = 0;
	clock.m = 0;
	clock.h = 0;

	return clock;
}
Lab Load_Lab(Map map, int x, int y) {
	Lab lab;
	lab.bitmap = al_load_bitmap("Bitmaps\\GameObjects\\Lab\\Lab.bmp");
	al_convert_mask_to_alpha(lab.bitmap, al_map_rgb(255, 0, 255));

	lab.bitmap_active = al_load_bitmap("Bitmaps\\GameObjects\\Halo\\Halo_Medium.bmp");
	al_convert_mask_to_alpha(lab.bitmap_active, al_map_rgb(255, 0, 255));

	lab.panel.icon = al_load_bitmap("Bitmaps\\GameObjects\\Lab\\Lab_Icon.bmp");
	al_convert_mask_to_alpha(lab.panel.icon, al_map_rgb(255, 0, 255));

	lab.pos_x = x;
	lab.pos_y = y;

	lab.panel = Load_Panel(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	lab.panel.icon = al_load_bitmap("Bitmaps\\GameObjects\\Lab\\Lab_Icon.bmp");
	al_convert_mask_to_alpha(lab.panel.icon, al_map_rgb(255, 0, 255));
	lab.panel.name = "Laboratory";
	
	lab.width = lab.width * map.tiles[0][0].width;
	lab.height = lab.height * map.tiles[0][0].height;

	lab.status_active = 0;
	return lab;
}
Bunker Load_Bunker(Map map, int x, int y) {
	Bunker bunker;
	bunker.bitmap = al_load_bitmap("Bitmaps\\GameObjects\\Bunker\\Bunker.bmp");
	al_convert_mask_to_alpha(bunker.bitmap, al_map_rgb(255, 0, 255));

	bunker.bitmap_active = al_load_bitmap("Bitmaps\\GameObjects\\Halo\\Halo_Medium.bmp");
	al_convert_mask_to_alpha(bunker.bitmap_active, al_map_rgb(255, 0, 255));

	bunker.status_active = 0;

	bunker.pos_x = x;
	bunker.pos_y = y;

	bunker.panel = Load_Panel(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	bunker.panel.icon = al_load_bitmap("Bitmaps\\GameObjects\\Bunker\\Bunker_Icon.bmp");
	al_convert_mask_to_alpha(bunker.panel.icon, al_map_rgb(255, 0, 255));
	bunker.panel.name = "Bunker";

	bunker.width = bunker.width * map.tiles[0][0].width;
	bunker.height = bunker.height * map.tiles[0][0].height;

	return bunker;
}
BuildUp Load_BuildUp(Map map, int type, int x, int y) {

	BuildUp buildup;

	buildup.bitmap = al_load_bitmap("Bitmaps\\GameObjects\\BuildUp\\BuildUp.bmp");
	al_convert_mask_to_alpha(buildup.bitmap, al_map_rgb(255, 0, 255));

	buildup.bitmap_active = al_load_bitmap("Bitmaps\\GameObjects\\Halo\\Halo_Medium.bmp");
	al_convert_mask_to_alpha(buildup.bitmap_active, al_map_rgb(255, 0, 255));

	buildup.panel = Load_Panel(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

	buildup.panel.icon = al_load_bitmap("Bitmaps\\GameObjects\\BuildUp\\BuildUp_Icon.bmp");
	al_convert_mask_to_alpha(buildup.panel.icon, al_map_rgb(255, 0, 255));
	buildup.panel.name = "Building Site";

	buildup.type = type;

	buildup.width = buildup.width * map.tiles[0][0].width;
	buildup.height = buildup.height * map.tiles[0][0].height;

//	printf("%i %i \n", x, y);

	buildup.pos_x = x * map.tiles[0][0].width;
	buildup.pos_y = y * map.tiles[0][0].height;

	map.tiles[y][x].type = 98;
	map.tiles[y + 1][x + 1].type = 98; 
	map.tiles[y + 1][x].type = 98; 
	map.tiles[y][x + 1].type = 98;

	buildup.status_active = 0;

	return buildup;
}
////////////////////////////////////////////////////

/////////////////Draw///////////////////////////////
void Draw_Interface(Interface interface)
{
	al_draw_bitmap(interface.bitmap, 0, 0, 0);
}
void Draw_Minimap(Map map, Interface interface, int mini_map_x, int mini_map_y) {
	int ih;
	int iw;

	al_draw_filled_rectangle(interface.mini_map.x, interface.mini_map.y,
							interface.mini_map.x + interface.mini_map.width, interface.mini_map.y + interface.mini_map.height,
							al_map_rgb(0, 0, 0));
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
			case 22:
				al_draw_filled_rectangle(interface.mini_map.x + iw * 5, interface.mini_map.y + ih * 5,
					interface.mini_map.x + iw * 5 + 5, interface.mini_map.y + ih * 5 + 5,
					al_map_rgb(255, 0, 0));
				break;
			case 91:
				al_draw_filled_rectangle(interface.mini_map.x + iw * 5, interface.mini_map.y + ih * 5,
					interface.mini_map.x + iw * 5 + 5, interface.mini_map.y + ih * 5 + 5,
					al_map_rgb(141, 54, 54));
				break;
			case 98:
				al_draw_filled_rectangle(interface.mini_map.x + iw * 5, interface.mini_map.y + ih * 5,
					interface.mini_map.x + iw * 5 + 5, interface.mini_map.y + ih * 5 + 5,
					al_map_rgb(255, 0, 0));
				break;

			case 99:
				al_draw_filled_rectangle(interface.mini_map.x + iw * 5, interface.mini_map.y + ih * 5,
					interface.mini_map.x + iw * 5 + 5, interface.mini_map.y + ih * 5 + 5,
					al_map_rgb(255, 0, 0));
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
			case 22:
				al_draw_bitmap(tile_bitmaps[2], map_x + iw*map.tiles[ih][iw].width, map_y + ih*map.tiles[ih][iw].height, 0);
				break;
			case 91:
				al_draw_bitmap(tile_bitmaps[2], map_x + iw*map.tiles[ih][iw].width, map_y + ih*map.tiles[ih][iw].height, 0);
				break;
			case 98:
				al_draw_bitmap(tile_bitmaps[1], map_x + iw*map.tiles[ih][iw].width, map_y + ih*map.tiles[ih][iw].height, 0);
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
void Draw_Cityhall(Cityhall cityhall, Map map, int map_x, int map_y){
	al_draw_bitmap(cityhall.bitmap, cityhall.pos_x + map_x, cityhall.pos_y + map_y, 0);
}
void Draw_Active_Cityhall(Cityhall cityhall, Map map, int map_x, int map_y){
	al_draw_bitmap(cityhall.bitmap_active, cityhall.pos_x + map_x, cityhall.pos_y + map_y, 0);
	al_draw_bitmap(cityhall.bitmap, cityhall.pos_x + map_x, cityhall.pos_y + map_y, 0);
}
void Draw_Goblin(Goblin goblin, Map map, int map_x, int map_y){
	al_draw_bitmap(goblin.bitmap, goblin.pos_x + map_x, goblin.pos_y + map_y, 0);
}
void Draw_Active_Goblin(Goblin goblin, Map map, int map_x, int map_y) {
	al_draw_bitmap(goblin.bitmap_active, goblin.pos_x + map_x, goblin.pos_y + map_y, 0);
	al_draw_bitmap(goblin.bitmap, goblin.pos_x + map_x, goblin.pos_y + map_y, 0);
}
void Draw_Lab(Lab lab, Map map, int map_x, int map_y) {
	al_draw_bitmap(lab.bitmap, lab.pos_x + map_x, lab.pos_y + map_y, 0);
}
void Draw_Active_Lab(Lab lab, Map map, int map_x, int map_y) {
	al_draw_bitmap(lab.bitmap_active, lab.pos_x + map_x, lab.pos_y + map_y, 0);
	al_draw_bitmap(lab.bitmap, lab.pos_x + map_x, lab.pos_y + map_y, 0);
}
void Draw_Bunker(Bunker bunker, Map map, int map_x, int map_y){
	al_draw_bitmap(bunker.bitmap, bunker.pos_x + map_x, bunker.pos_y + map_y, 0);
}
void Draw_Active_Bunker(Bunker bunker, Map map, int map_x, int map_y){
	al_draw_bitmap(bunker.bitmap_active, bunker.pos_x + map_x, bunker.pos_y + map_y, 0);
	al_draw_bitmap(bunker.bitmap, bunker.pos_x + map_x, bunker.pos_y + map_y, 0);
}
void Draw_Rider(Rider rider, Map map, int map_x, int map_y) {
	al_draw_bitmap(rider.bitmap, rider.pos_x + map_x, rider.pos_y + map_y, 0);
}
void Draw_Active_Rider(Rider rider, Map map, int map_x, int map_y) {
	al_draw_bitmap(rider.bitmap_active, rider.pos_x + map_x, rider.pos_y + map_y, 0);
	al_draw_bitmap(rider.bitmap, rider.pos_x + map_x, rider.pos_y + map_y, 0);
}
void Draw_Button(Button button) {
	al_draw_bitmap(button.bitmap,button.x, button.y, 0);
}
void Draw_Panel(Panel *panel, Interface interface, ALLEGRO_FONT *font) {
		al_draw_filled_rectangle(interface.status.x, interface.status.y,
			interface.status.x + interface.status.width, interface.status.y + interface.status.height,
			al_map_rgb(0, 0, 0));
		al_draw_filled_rectangle(interface.actions.x, interface.actions.y,
			interface.actions.x + interface.actions.width, interface.actions.y + interface.actions.height,
			al_map_rgb(0, 0, 0));
	if (panel != nullptr) {
		if (panel->icon != NULL) {
			al_draw_bitmap(panel->icon, interface.status.x + 15, interface.status.y + 15, 0);
		}

		al_draw_textf(font, al_map_rgb(255, 255, 255), interface.status.x + 120, interface.status.y + 15, 0, "%s ", (panel->name).c_str());
		if (panel->health_max != NULL) {
			al_draw_textf(font, al_map_rgb(255, 255, 255), interface.status.x + 120, interface.status.y + 40, 0, "%i / %i ", panel->health_current, panel->health_max);
		}

		if (panel->button00.bitmap) {
			Draw_Button(panel->button00);
		}
		if (panel->button01.bitmap) {
			Draw_Button(panel->button01);
		}
		if (panel->button02.bitmap) {
			Draw_Button(panel->button02);
		}
		if (panel->button10.bitmap) {
			Draw_Button(panel->button10);
		}
		if (panel->button11.bitmap) {
			Draw_Button(panel->button11);
		}
		if (panel->button12.bitmap) {
			Draw_Button(panel->button12);
		}
		if (panel->button20.bitmap) {
			Draw_Button(panel->button20);
		}
		if (panel->button21.bitmap) {
			Draw_Button(panel->button21);
		}
		if (panel->button22.bitmap) {
			Draw_Button(panel->button22);
		}
	}
}
void Draw_Path(list<Tile*> path, Map map, int map_x, int map_y) {
	Tile *a;
	Tile *b;
	int ax;
	int bx;
	int ay;
	int by;
	for (list<Tile*>::iterator iter = path.begin(), prev = path.end(); iter != path.end(); prev = iter, iter++) {
		if(prev != path.end()){
			a = *prev;
			b = *iter;
			ax = (a->x)*(a->width) + (a->width / 2) + map_x;
			ay = (a->y)*(a->height) + (a->height / 2) + map_y;
			bx = (b->x)*(b->width) + (b->width / 2) + map_x;
			by = (b->y)*(b->height) + (b->height / 2) + map_y;
			al_draw_line(ax, ay, bx, by, al_map_rgb(0, 0, 0), 3);
		}
	}
}
void Draw_Timer(Clock clock, ALLEGRO_FONT *font) {
	al_draw_textf(font, al_map_rgb(255, 255, 255), 10, 10, 0, "%i : %i : %i ", clock.h, clock.m, clock.s);
}
void Draw_BuildUp(BuildUp buildup, Map map, int map_x, int map_y) {
	al_draw_bitmap(buildup.bitmap, buildup.pos_x + map_x, buildup.pos_y + map_y, 0);
}
void Draw_Active_BuildUp(BuildUp buildup, Map map, int map_x, int map_y) {
	al_draw_bitmap(buildup.bitmap_active, buildup.pos_x + map_x, buildup.pos_y + map_y, 0);
	al_draw_bitmap(buildup.bitmap, buildup.pos_x + map_x, buildup.pos_y + map_y, 0);
}
////////////////////////////////////////////////////

//////////////////CursorPosition////////////////////
bool Cursor_On_MainMap(Cursor cursor, Interface interface) 
{
	if ((cursor.x >= interface.main_map.x && cursor.x <= interface.main_map.x + interface.main_map.width) &&
		(cursor.y >= interface.main_map.y && cursor.y <= interface.main_map.y + interface.main_map.height))
	{
		//printf("on_map \n");
		return true;
	}
	return false;
}
bool Cursor_On_Panel(Cursor cursor, Interface interface)
{
	if ((cursor.x >= interface.actions.x && cursor.x <= interface.actions.x + interface.actions.width) &&
		(cursor.y >= interface.actions.y && cursor.y <= interface.actions.y + interface.actions.height))
	{
	//	printf("on_panel \n");
		return true;
	}
	return false;
}
bool Cursor_on_Item(Cursor cursor, int x, int y, int width, int height)
{
	if ((cursor.x >= x && cursor.x <= x + width) &&
		(cursor.y >= y && cursor.y <= y + height))
	{
		//printf("on_item \n");
		return true;
	}
	return false;
}
///////////////////////////////////////////////////////

////////////////Activation/Desactivation/////////////////////
void Set_Cityhall_Active(Cityhall *cityhall, Cityhall *active_cityhall)
{
	cityhall->status_active = 1;
}
void Set_Cityhall_InActive(Cityhall *cityhall, Cityhall *active_cityhall)
{
	cityhall->status_active = 0;
}
void Set_Goblin_Active(Goblin *goblin, Goblin *active_goblin)
{
	goblin->status_active = 1;
}
void Set_Goblin_InActive(Goblin *goblin, Goblin *active_goblin)
{
	goblin->status_active = 0;
}
void Set_Bunker_Active(Bunker *bunker, Bunker *active_bunker)
{
	bunker->status_active = 1;
}
void Set_Bunker_InActive(Bunker *bunker, Bunker *active_bunker)
{
	bunker->status_active = 0;
}
void Set_Lab_Active(Lab *lab, Lab *active_lab)
{
	lab->status_active = 1;
}
void Set_Lab_InActive(Lab *lab, Lab *active_lab)
{
	lab->status_active = 0;
}
void Set_Rider_Active(Rider *rider, Rider *active_rider)
{
	rider->status_active = 1;
}
void Set_Rider_InActive(Rider *rider, Rider *active_rider)
{
	rider->status_active = 0;
}
void Update_Clock(Clock *clock) {
	clock->t = clock->t + 1;

	if (clock->t == 60) {
		clock->s = clock->s + 1;
		clock->t = 0;
	}

	if (clock->s == 60) {
		clock->m = clock->m + 1;
		clock->s = 0;
	}

	if (clock->m == 60) {
		clock->h = clock->h + 1;
		clock->m = 0;
	}
}
void Set_BuildUp_Active(BuildUp *buildup, BuildUp *active_buildup)
{
	buildup->status_active = 1;
}
void Set_BuildUp_InActive(BuildUp *buildup, BuildUp *active_buildup)
{
	buildup->status_active = 0;
}
void Set_Panel_InActive(Panel * panel) {
	panel->button00.bitmap = NULL;
	panel->button01.bitmap = NULL;
	panel->button02.bitmap = NULL;
	panel->button10.bitmap = NULL;
	panel->button11.bitmap = NULL;
	panel->button12.bitmap = NULL;
	panel->button20.bitmap = NULL;
	panel->button21.bitmap = NULL;
	panel->button22.bitmap = NULL;
	panel->icon = NULL;
}
///////////////////////////////////////////////////////////

//////////////////A_star Pathfinding///////////////////////
typedef struct node {
	Tile *tile;
	node *parent = NULL;
	int h = 0;
	int g = 0;
	int f = 0;
	bool o = false;
	bool c = false;
	node *adjacent[4] = { NULL, NULL, NULL, NULL };
}node;
typedef bool(*checkpath)(node* node);
bool Check_f_goblin(node *node) {
	if (node->tile->type % 10 != 1) return true;
	return false;
};
bool Check_f_rider(node *node) {
	if (node->tile->type != 21 && node->tile->type != 91) return true;
	return false;
}
bool node_in_list(node *n, list<node*> l){
	for (list<node*>::iterator iter = l.begin(); iter != l.end(); iter++) {
		if (*iter == n && *iter == n) return true;
	}
	return false;
}
node* lowest_node(list<node*> lista) {

	lista.sort([](node *first, node *second) {return first->f < second->f; });
	node *n = lista.front();

	return n;
}
list<Tile*> a_star_path(int t_start_x, int t_start_y, int t_end_x, int t_end_y, Map map, std::function<bool(node*)> checkpath)
{
	list<Tile*> path;
	int h, w, main_h;
	int hx = 1;
	list<node*> open;
	node **nodes;
	nodes = new node*[map.height];
	for (h = 0; h < map.height; h++) {
		nodes[h] = new node[map.width];
		for (w = 0; w < map.width; w++) {
			nodes[h][w].tile = &(map.tiles[h][w]);
			if (map.tiles[h][w].type % 10 == 1) {
				nodes[h][w].h = fabs(map.tiles[t_end_y][t_end_x].x*hx - map.tiles[h][w].x*hx) + fabs(map.tiles[t_end_y][t_end_x].y*hx - map.tiles[h][w].y*hx);
			}
			else {
				nodes[h][w].h = 10000;
			}
		}
	}
	for (h = 0; h < map.height; h++) {
		for (w = 0; w < map.width; w++) {
			if (h > 0) nodes[h][w].adjacent[0] = &nodes[h - 1][w];
			if(h < map.height - 1) nodes[h][w].adjacent[1] = &nodes[h + 1][w];
			if(w > 0) nodes[h][w].adjacent[2] = &nodes[h][w - 1];
			if(w < map.width - 1) nodes[h][w].adjacent[3] = &nodes[h][w + 1];
		}
	}
	open.push_front(&nodes[t_start_y][t_start_x]);
	node *Q;
	int new_G;
	while (!open.empty()) {
		open.sort([](node *first, node *second) {return first->f < second->f; });
		Q = open.front();
		Q->c = true;
		if (Q->tile->x == t_end_x && Q->tile->y == t_end_y) {
			break;
		}
		open.pop_front();
		for (int i = 0; i < 4; i++) {
			if (Q->adjacent[i] != NULL) {
				if (Q->adjacent[i]->c == 1 || checkpath(Q->adjacent[i])) {
					continue;
				}
				else if (node_in_list(Q->adjacent[i], open) == false) {
					open.push_front(Q->adjacent[i]);
					Q->adjacent[i]->parent = Q;
					Q->adjacent[i]->g = Q->adjacent[i]->g + 10;
					Q->adjacent[i]->f = Q->adjacent[i]->g + Q->adjacent[i]->h;
				}
				else {
					new_G = Q->adjacent[i]->g + 10;
					if (new_G < Q->adjacent[i]->g) {
						Q->adjacent[i]->parent = Q;
						Q->adjacent[i]->g = new_G;
						Q->adjacent[i]->f = Q->adjacent[i]->g + Q->adjacent[i]->h;
					}
				}
			}
			else {
				continue;
			}
		}
	}
	Q = &nodes[t_end_y][t_end_x];
	while(true) {
		if (Q->parent == NULL)
			break;
		path.push_front(Q->tile);
		Q = Q->parent;
	}
	return path;
}
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
void Goblin_Build(Goblin *goblin, Map map, int type, int x, int y) {
	if (map.tiles[y][x].type == 11 && map.tiles[y+1][x].type == 11 && map.tiles[y][x+1].type == 11 && map.tiles[y+1][x+1].type == 11) {
		goblin->path = a_star_path(goblin->tile_pos_x, goblin->tile_pos_y, x, y, map, Check_f_goblin);
		goblin->busy = type;
	}
}
///////////////////////////////////////////////////////////

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
	al_init_font_addon();
	al_init_ttf_addon();
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

	ALLEGRO_FONT *font24 = al_load_font("arial.ttf", 24, 0);
	ALLEGRO_FONT *font16 = al_load_font("arial.ttf", 16, 0);

	// Cursor
	Cursor cursor = Load_Cursor(interface);
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
	Cityhall cityhall = Load_Cityhall(map);
	list<Goblin> goblins;
	list<Bunker> bunkers;
	list<Lab> labs;
	list<BuildUp> buildups;
	list<Rider> riders;
	list<Rider> buffor_riders;

	Clock clock = Load_Clock();
	Clock* cp = &clock;

	// Pirszy Gobo
	Goblin goblin = Load_Goblin(map, cityhall.entrance_x, cityhall.entrance_y);
	goblins.push_front(goblin);

	//Control Variables
	Goblin * active_goblin = NULL;
	Bunker * active_bunker = NULL;
	Rider * active_rider = NULL; 
	BuildUp * active_buildup = NULL;
	Cityhall * active_cityhall = NULL;
	Panel * active_panel = NULL;
	Lab * active_lab = NULL;

	int rider_count = 20;
	float rider_speed = 0.8;
	int rider_health = 20;

	
	Rider current_rider;
	BuildUp current_buildup;
	Bunker current_bunker;
	Goblin current_goblin;
	Lab current_lab;

	Tile* t;

	int goblin_s = 0;
	int bunker_s = 0;
	int rider_s = 0;
	int lab_s = 0;
	int bups_s = 0;

	float dist;

	int kill_count = 0;
	int money = 500;

	int temp_x, temp_y;
	int temp_t;

	while (!done) {
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		//Events
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

			if (Cursor_On_MainMap(cursor, interface) == true) {
				cursor.t_x = fabs((map_x - cursor.x) / (map.tiles[0][0].width));
				cursor.t_y = fabs((map_y - cursor.y) / (map.tiles[0][0].height));
			}

			redraw = true;
		}
		else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			switch (ev.mouse.button){
			case 1:
				cursor.buttons[0] = true;
				if (Cursor_On_MainMap(cursor, interface) == true && cursor.cursor_task == 0){
					for (list<Goblin>::iterator iter = goblins.begin(); iter != goblins.end(); iter++) {
						if (Cursor_on_Item(cursor, iter->pos_x + map_x, iter->pos_y + map_y, iter->width, iter->height) == true) {
							Set_Goblin_Active(&*iter, active_goblin);
							active_goblin = &(*iter);
							active_panel = &active_goblin->panel;
							goblin_s = 1;
						}
						else {
							Set_Goblin_InActive(&*iter, active_goblin);
							if (goblin_s != 1) {
								active_goblin = NULL;
							}
						}
					}
					goblin_s = 0;
					for (list<Bunker>::iterator iter = bunkers.begin(); iter != bunkers.end(); iter++) {
						if (Cursor_on_Item(cursor, iter->pos_x + map_x, iter->pos_y + map_y, iter->width, iter->height) == true) {
							Set_Bunker_Active(&*iter, active_bunker);
							active_bunker = &(*iter);
							active_panel = &*(&iter->panel);
							bunker_s = 1;
						}
						else {
							Set_Bunker_InActive(&*iter, active_bunker);
							if (bunker_s != 1) {
								active_bunker = NULL;
							}

						}
					}
					bunker_s = 0;
					for (list<Rider>::iterator iter = riders.begin(); iter != riders.end(); iter++) {
						if (Cursor_on_Item(cursor, iter->pos_x + map_x, iter->pos_y + map_y, iter->width, iter->height) == true) {
							Set_Rider_Active(&*iter, active_rider);
							active_rider = &(*iter);
							active_panel = &(iter->panel);
							rider_s = 1;
						}
						else {
							Set_Rider_InActive(&*iter, active_rider);
							if (rider_s != 1) {
								active_rider = NULL;
							}
						}
					}
					rider_s = 0;
					for (list<Lab>::iterator iter = labs.begin(); iter != labs.end(); iter++) {
						if (Cursor_on_Item(cursor, iter->pos_x + map_x, iter->pos_y + map_y, iter->width, iter->height) == true) {
							Set_Lab_Active(&*iter, active_lab);
							active_lab = &(*iter);
							active_panel = &(iter->panel);
							lab_s = 1;
						}
						else {
							Set_Lab_InActive(&*iter, active_lab);
							if (lab_s != 1) {
								active_lab = NULL;
							}
						}
					}
					lab_s = 0;
					for (list<BuildUp>::iterator iter = buildups.begin(); iter != buildups.end(); iter++) {

						if (Cursor_on_Item(cursor, iter->pos_x + map_x, iter->pos_y + map_y, iter->width, iter->height) == true) {
							Set_BuildUp_Active(&*iter, active_buildup);
							active_buildup = &(*iter);
							active_panel = &(iter->panel);
							bups_s = 1;
						}
						else {
							Set_BuildUp_InActive(&*iter, active_buildup);
							if (bups_s != 1) {
								active_buildup = NULL;
							}
						}
					}
					bups_s = 0;

					if (Cursor_on_Item(cursor, cityhall.pos_x + map_x, cityhall.pos_y + map_y, cityhall.width, cityhall.height) == true) {
						Set_Cityhall_Active(&cityhall, active_cityhall);
						active_cityhall = &cityhall;
						active_panel = &active_cityhall->panel;
					}
					else {
						Set_Cityhall_InActive(&cityhall, active_cityhall);
						active_cityhall = NULL;
					}
				}
				else if (Cursor_On_MainMap(cursor, interface) == true && cursor.cursor_task != 0) {
					if (cursor.cursor_task == 1 && active_goblin != NULL) {
						Goblin_Build(active_goblin, map, 1, cursor.t_x, cursor.t_y);
						cursor.cursor_task = 0;
					}
					else if (cursor.cursor_task == 2 && active_goblin != NULL) {
						Goblin_Build(active_goblin, map, 2, cursor.t_x, cursor.t_y);
						cursor.cursor_task = 0;
					}
				}
				else if (Cursor_On_Panel(cursor, interface) == true) {
				/////////////////////////Panels Handling///////////////////////////////
					if (active_goblin != NULL) {
						if (Cursor_on_Item(cursor, active_goblin->panel.button00.x , active_goblin->panel.button00.y, 100, 100) == true) {
							cursor.cursor_task = 1;
						}
						else if((Cursor_on_Item(cursor, active_goblin->panel.button01.x, active_goblin->panel.button00.y, 100, 100) == true)) {
							cursor.cursor_task = 2;
						}
					}
					else if(active_cityhall != NULL) {
						if (Cursor_on_Item(cursor, active_cityhall->panel.button00.x, active_cityhall->panel.button00.y, 100, 100) == true) {
							printf("on_item1 \n");
						}
						else if (Cursor_on_Item(cursor, active_cityhall->panel.button01.x, active_cityhall->panel.button01.y, 100, 100) == true) {
							printf("on_item2 \n");
						}
						else if (Cursor_on_Item(cursor, active_cityhall->panel.button02.x, active_cityhall->panel.button02.y, 100, 100) == true) {
							printf("on_item3 \n");
						}
					}
					else if (active_lab != NULL) {

					}
					else if (active_bunker != NULL) {

					}
				}
				
				break;
			case 2:
				cursor.buttons[1] = true;
				if (active_goblin != NULL) {
					active_goblin->path = a_star_path(active_goblin->tile_pos_x, active_goblin->tile_pos_y, cursor.t_x, cursor.t_y, map, Check_f_goblin);
					active_goblin->busy = 0;
				}
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
			//State Calculation
			if (goblins.empty() != true) {
				for (list<Goblin>::iterator iter = goblins.begin(); iter != goblins.end();) {
					if (!(iter->path).empty()) {
						t = *(iter->path.begin());
						dist = sqrt(((t->x)*(t->width) - iter->pos_x)*((t->x)*(t->width) - iter->pos_x) + ((t->y)*(t->width) - iter->pos_y)*((t->y)*(t->height) - iter->pos_y));

						if (dist <= iter->speed) {
							iter->pos_x = (t->x)*(t->width);
							iter->pos_y = (t->y)*(t->height);
							iter->tile_pos_x = t->x;
							iter->tile_pos_y = t->y;
							iter->path.pop_front();
							continue;
						}

						if ((t->x)*(t->width) < iter->pos_x) {
							iter->pos_x = iter->pos_x - iter->speed;
						}
						else if ((t->x)*(t->width) > iter->pos_x) {
							iter->pos_x = iter->pos_x + iter->speed;
						}
						else if ((t->y)*(t->width) < iter->pos_y) {
							iter->pos_y = iter->pos_y - iter->speed;
						}
						else if ((t->y)*(t->width) > iter->pos_y) {
							iter->pos_y = iter->pos_y + iter->speed;
						}
						++iter;
					}
					else if ((iter->path).empty() && iter->busy != 0) {
						temp_x = iter->tile_pos_x;
						temp_y = iter->tile_pos_y;
						temp_t = iter->busy;
						if (active_goblin == &*iter) {
							Set_Goblin_InActive(&*iter, active_goblin);
							active_goblin = NULL;
						}
						Set_Panel_InActive(&(iter->panel));
						iter = goblins.erase(iter);
						if (temp_t == 1) {
							current_buildup = Load_BuildUp(map, 1, temp_x, temp_y);
							buildups.push_back(current_buildup);
						}
						else if (temp_t == 2) {
							current_buildup = Load_BuildUp(map, 2, temp_x, temp_y);
							buildups.push_back(current_buildup);
						}

					}
					else {
						++iter;
					}
				}
			}

			if (bunkers.empty() != true) {
				for (list<Bunker>::iterator iter = bunkers.begin(); iter != bunkers.end(); iter++) {					
					for (list<Rider>::iterator a = riders.begin(); a != riders.end(); a++) {
						if ( sqrt((iter->pos_x + 50 - a->pos_x)*(iter->pos_x + 50 - a->pos_x) + (iter->pos_y + 50 - a->pos_y)*(iter->pos_y + 50 - a->pos_y))  <= iter->range * 50) {
							(&*iter)->target = &*a;
							break;
						}
						else if(&*iter->target != NULL) {
							(&*iter)->target = NULL;
						}
					}

					if (iter->target != NULL && iter->target->panel.health_current > 0) {
						if (iter->cooldown > 0) {
							(&*iter)->cooldown = iter->cooldown--;
						}
						else if(iter->cooldown == 0){
							*&iter->target->panel.health_current = *&iter->target->panel.health_current - 10;

							(&*iter)->cooldown = 90;
						}
					}
					else if (iter->target == NULL) {
						(&*iter)->cooldown = 90;
					}

				}
			}

			if (labs.empty() != true) {
				for (list<Lab>::iterator iter = labs.begin(); iter != labs.end(); iter++) {

				}
			}

			if (buildups.empty() != true) {
				for (list<BuildUp>::iterator iter = buildups.begin(); iter != buildups.end();) {
					if (iter->partial < iter->full) { 
						iter->partial++;
						++iter;
					}
					else {
						temp_x = iter->pos_x;
						temp_y = iter->pos_y;
						temp_t = iter->type;
						if (active_buildup == &*iter) {
							Set_BuildUp_InActive(&*iter, active_buildup);
							active_buildup = NULL;
						}
						Set_Panel_InActive(&(iter->panel));
						iter = buildups.erase(iter);
						if (temp_t == 1) {
							current_bunker = Load_Bunker(map, temp_x, temp_y);
							bunkers.push_back(current_bunker);
						}
						else if (temp_t == 2) {
							current_lab = Load_Lab(map, temp_x, temp_y);
							labs.push_back(current_lab);
						}

						current_goblin = Load_Goblin(map, (temp_x / 50), (temp_y / 50) + 2);
						goblins.push_back(current_goblin);
					}
				}
			}

			if (riders.empty() != true){
				for (list<Rider>::iterator iter = riders.begin(); iter != riders.end();) {
					if (iter->status_active == true && active_panel == nullptr) {
						active_panel = &iter->panel;
					}

					if (iter->panel.health_current <= 0) {
						if (active_rider == &*iter) {
							Set_Rider_InActive(&*iter, active_rider);
							active_rider = NULL;
						}
						Set_Panel_InActive(&(iter->panel));
						iter = riders.erase(iter);
						kill_count = kill_count++;
					}
					else if ((iter->path).empty()) {
						if (active_rider == &*iter) {
							Set_Rider_InActive(&*iter, active_rider);
							active_rider = NULL;
						}
						Set_Panel_InActive(&(iter->panel));
						iter = riders.erase(iter);
						cityhall.panel.health_current = cityhall.panel.health_current - 10;
					}
					else if (!(iter->path).empty()) {
						t = *(iter->path.begin());
						dist = sqrt(((t->x)*(t->width) - iter->pos_x)*((t->x)*(t->width) - iter->pos_x) + ((t->y)*(t->width) - iter->pos_y)*((t->y)*(t->height) - iter->pos_y));
						if (dist <= iter->speed) {
							iter->pos_x = t->x * t->width;
							iter->pos_y = t->y * t->height;
							iter->tile_pos_x = t->x;
							iter->tile_pos_y = t->y;
							iter->path.pop_front();
							continue;
						}
						if ((t->x)*(t->width) < iter->pos_x) {
							iter->pos_x = iter->pos_x - iter->speed;
						}
						else if ((t->x)*(t->width) > iter->pos_x) {
							iter->pos_x = iter->pos_x + iter->speed;
						}
						else if ((t->y)*(t->width) < iter->pos_y) {
							iter->pos_y = iter->pos_y - iter->speed;
						}
						else if ((t->y)*(t->width) > iter->pos_y) {
							iter->pos_y = iter->pos_y + iter->speed;
						}
						++iter;
					}
				}
			}

			if ((clock.t == 0) && (clock.s == 0) && (clock.m >= 1)) {
				for (int i = 0; i < rider_count; i++) {
					current_rider = Load_Rider(map, map.s_x, map.s_y, rider_health, rider_speed);
					buffor_riders.push_back(current_rider);
				}

				if (rider_count < 25) rider_count++;
				if (rider_speed < 15) rider_speed = rider_speed + 0.25;
				rider_health = rider_health + 0.25 * rider_health;
			}

			if ((clock.t == 0) && (clock.s % 2 == 1) && !(buffor_riders.empty()) ) {
				current_rider = buffor_riders.front();
				current_rider.path = a_star_path(map.s_x, map.s_y, 17, 19, map, Check_f_rider);
				riders.push_back(current_rider);
				buffor_riders.pop_front();
			}

			Update_Clock(cp);
			redraw = true;
		}
		 
		//Drawing 
		if (redraw && al_is_event_queue_empty(event_queue)) {
			redraw = false;

			Draw_Map(map, tile_bitmaps, map_x, map_y);
			if (cityhall.status_active == 1) {
				Draw_Active_Cityhall(cityhall, map, map_x, map_y);
			}
			else {
				Draw_Cityhall(cityhall, map, map_x, map_y);
			}

			for (list<Goblin>::iterator iter = goblins.begin(); iter != goblins.end(); iter++) {
				if (iter->status_active == 1) {
					Draw_Path(*(&iter->path), map, map_x, map_y);
					Draw_Active_Goblin(*iter, map, map_x, map_y);
				}
				else {
					Draw_Goblin(*iter, map, map_x, map_y);
				}
			}

			for (list<Lab>::iterator iter = labs.begin(); iter != labs.end(); iter++) {
				if (iter->status_active == 1) {
					Draw_Active_Lab(*iter, map, map_x, map_y);
				}
				else {
					Draw_Lab(*iter, map, map_x, map_y);
				}

			}
			
			for (list<BuildUp>::iterator iter = buildups.begin(); iter != buildups.end(); iter++) {
				if (iter->status_active == 1) {
					Draw_Active_BuildUp(*iter, map, map_x, map_y);
				}
				else {
					Draw_BuildUp(*iter, map, map_x, map_y);
				}
			}

			for (list<Rider>::iterator iter = riders.begin(); iter != riders.end(); iter++) {
				if (iter->status_active == 1) {
					Draw_Active_Rider(*iter, map, map_x, map_y);
				}
				else {
					Draw_Rider(*iter, map, map_x, map_y);
				}
			}

			for (list<Bunker>::iterator iter = bunkers.begin(); iter != bunkers.end(); iter++) {
				if (iter->status_active == 1) {
					Draw_Active_Bunker(*iter, map, map_x, map_y);
				}
				else {
					Draw_Bunker(*iter, map, map_x, map_y);
				}

				if (&*iter->target != NULL && iter->target->panel.health_current > 0) {
					al_draw_line(iter->pos_x + 50 + map_x, iter->pos_y + 10 + map_y, iter->target->pos_x + map_x + 25, iter->target->pos_y + map_y + 25, al_map_rgb(200, 200, 0), 3);
					if (iter->cooldown >= 0 && iter->cooldown <= 30) {
						al_draw_line(iter->pos_x + 50 + map_x, iter->pos_y + 10 + map_y, iter->target->pos_x + map_x + 25, iter->target->pos_y + map_y + 25, al_map_rgb(200, 0, 0), 3);
					}
				}

			}

			Draw_Interface(interface);
			Draw_Minimap(map, interface, mini_map_x, mini_map_y);
			if (active_cityhall != NULL || active_goblin != NULL || active_bunker != NULL || active_rider != NULL || active_lab != NULL || active_buildup != NULL) {
				Draw_Panel(active_panel, interface, font16);
			}
			else{
				al_draw_filled_rectangle(interface.status.x, interface.status.y,
										interface.status.x + interface.status.width, interface.status.y + interface.status.height,
										al_map_rgb(0, 0, 0));
				al_draw_filled_rectangle(interface.actions.x, interface.actions.y,
										interface.actions.x + interface.actions.width, interface.actions.y + interface.actions.height,
										al_map_rgb(0, 0, 0));
			}

			Draw_Timer(clock, font24);

			al_draw_textf(font24, al_map_rgb(255, 255, 255), 300, 10, 0, "Kill count: %i", kill_count);
			al_draw_textf(font24, al_map_rgb(255, 255, 255), 600, 10, 0, "Sources: %i", money);
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
