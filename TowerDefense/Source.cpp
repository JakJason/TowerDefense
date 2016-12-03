#include <allegro5\allegro.h>
#include <allegro5\allegro_image.h>
#include <allegro5\allegro_primitives.h>
#include <string>
#include <fstream>
#include <cstdio>

using namespace std;
enum KEYS { UP, DOWN, LEFT, RIGHT };

int** Load_Map(string map, int* map_height, int* map_width)
{
	int** a2D = 0;
	int height;
	int width;

	ifstream f;
	f.open("Maps\\map1.txt");

	f >> height;
	f >> width;

	*map_height = height;
	*map_width = width;

	int h = 0;
	int w = 0;

	a2D = new int*[height];
	for (int h = 0; h < height; h++)
	{
		a2D[h] = new int[width];

		for (int w = 0; w < width; w++)
		{
			f >> a2D[h][w];
			printf("%i ", a2D[h][w]);
		}
		printf("\n");
	}
	return a2D;
}

int main(void)
{
	int width = 1600;
	int height = 900;
	int width_tile = 50;
	int height_tile = 50;

	int FPS = 60;

	int pos_0_x = 40;
	int pos_0_y = 50;
	int pos_K_x = 1190;
	int pos_K_y = 850;

	int pos_x = pos_0_x;
	int pos_y = pos_0_y;

	int minmap_pos_0_x = 1230;
	int minmap_pos_0_y = 50;
	int minmap_pos_K_x = 1560;
	int minmap_pos_K_y = 270;

	int window_pos_x = minmap_pos_0_x;
	int window_pos_y = minmap_pos_0_y;

	bool keys[4] = { false, false, false, false };
	bool done = false;

	//allegro variable
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_BITMAP *InterFace = NULL;

	ALLEGRO_BITMAP *Grass = NULL;
	ALLEGRO_BITMAP *Road = NULL;
	ALLEGRO_BITMAP *GrassRoad1 = NULL;
	ALLEGRO_BITMAP *GrassRoad2 = NULL;
	ALLEGRO_BITMAP *GrassRoad3 = NULL;
	ALLEGRO_BITMAP *GrassRoad4 = NULL;

	if (!al_init())										//initialize Allegro
		return -1;

	display = al_create_display(width, height);			//create our display object

	if (!display)										//test display object
		return -1;

	al_install_keyboard();
	al_install_mouse();
	al_init_image_addon();
	al_init_primitives_addon();

	InterFace = al_load_bitmap("Bitmaps/Interface/Interface.bmp");
	Grass = al_load_bitmap("Bitmaps/Tiles/Grass.bmp");
	GrassRoad1 = al_load_bitmap("Bitmaps/Tiles/GrassRoad1.bmp");
	GrassRoad2 = al_load_bitmap("Bitmaps/Tiles/GrassRoad2.bmp");
	GrassRoad3 = al_load_bitmap("Bitmaps/Tiles/GrassRoad3.bmp");
	GrassRoad4 = al_load_bitmap("Bitmaps/Tiles/GrassRoad4.bmp");
	Road = al_load_bitmap("Bitmaps/Tiles/Road.bmp");

	al_convert_mask_to_alpha(InterFace, al_map_rgb(255, 0, 255));
	if (!InterFace)
		return -1;

	event_queue = al_create_event_queue();
	timer = al_create_timer(1.0 / FPS);

	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_timer_event_source(timer));

	al_start_timer(timer);

	int h = 0;
	int w = 0;
	int ih = 0;
	int iw = 0;

	int *map_height;
	int *map_width;

	map_height = &h;
	map_width = &w;

	int window_h = 16;
	int window_w = 23;

	int** map = Load_Map("map2", map_height, map_width);

	int xyz = 11111000;

	int min_pos_x = pos_0_x;
	int min_pos_y = pos_0_y;
	int max_pos_x = -((w)*(width_tile) - pos_K_x);
	int max_pos_y = -((h)*(height_tile) - pos_K_y);

	int window_min_pos_x = minmap_pos_0_x;
	int window_min_pos_y = minmap_pos_0_y;
	int window_max_pos_x = minmap_pos_0_x + 5*(w-window_w);
	int window_max_pos_y = minmap_pos_0_y + 5*(h-window_h);



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
		else if (ev.type == ALLEGRO_EVENT_TIMER)
		{
			pos_y += keys[UP] * 10;
			pos_y -= keys[DOWN] * 10;
			pos_x += keys[LEFT] * 10;
			pos_x -= keys[RIGHT] * 10;

			window_pos_y -= keys[UP];
			window_pos_y += keys[DOWN];
			window_pos_x -= keys[LEFT];
			window_pos_x += keys[RIGHT];
		}
		if (pos_x >= min_pos_x) {
			window_pos_x = window_min_pos_x;
			pos_x = min_pos_x;
		}
		if (pos_x <= max_pos_x) {
			window_pos_x = window_max_pos_x;
			pos_x = max_pos_x;
		}
		if (pos_y >= min_pos_y) {
			window_pos_y = window_min_pos_y;
			pos_y = min_pos_y;
		}
		if (pos_y <= max_pos_y) {
			window_pos_y = window_max_pos_y;
			pos_y = max_pos_y;
		}

		printf("x: %i ", pos_x);
		printf("y: %i ", pos_y);
		printf("\n");

		for (int ih = 0; ih < *map_height; ih++)
		{
			for (int iw = 0; iw < *map_width; iw++)
			{
				switch (map[ih][iw]) {
				case 11:
					al_draw_bitmap(Grass, pos_x + iw*width_tile, pos_y + ih*height_tile, 0);
					break;
				case 21:
					al_draw_bitmap(Road, pos_x + iw*width_tile, pos_y + ih*height_tile, 0);
					break;
				case 31:
					al_draw_bitmap(GrassRoad1, pos_x + iw*width_tile, pos_y + ih*height_tile, 0);
					break;
				case 32:
					al_draw_bitmap(GrassRoad2, pos_x + iw*width_tile, pos_y + ih*height_tile, 0);
					break;
				case 33:
					al_draw_bitmap(GrassRoad3, pos_x + iw*width_tile, pos_y + ih*height_tile, 0);
					break;
				case 34:
					al_draw_bitmap(GrassRoad4, pos_x + iw*width_tile, pos_y + ih*height_tile, 0);
					break;
				}
			}

		}

		al_draw_bitmap(InterFace, 0, 0, 0);

		for (int ih = 0; ih < *map_height; ih++)
		{
			for (int iw = 0; iw < *map_width; iw++)
			{
				switch (map[ih][iw]) {
				case 11:
					al_draw_filled_rectangle(minmap_pos_0_x + iw * 5, minmap_pos_0_y + ih * 5,
						minmap_pos_0_x + iw * 5 + 5, minmap_pos_0_y + ih * 5 + 5,
						al_map_rgb(0, 255, 0));
					break;
				case 21:
					al_draw_filled_rectangle(minmap_pos_0_x + iw * 5, minmap_pos_0_y + ih * 5,
						minmap_pos_0_x + iw * 5 + 5, minmap_pos_0_y + ih * 5 + 5,
						al_map_rgb(141, 54, 54));
					break;
				case 31:
					al_draw_filled_rectangle(minmap_pos_0_x + iw * 5, minmap_pos_0_y + ih * 5,
						minmap_pos_0_x + iw * 5 + 5, minmap_pos_0_y + ih * 5 + 5,
						al_map_rgb(141, 54, 54));
					break;
				case 32:
					al_draw_filled_rectangle(minmap_pos_0_x + iw * 5, minmap_pos_0_y + ih * 5,
						minmap_pos_0_x + iw * 5 + 5, minmap_pos_0_y + ih * 5 + 5,
						al_map_rgb(141, 54, 54));
					break;
				case 33:
					al_draw_filled_rectangle(minmap_pos_0_x + iw * 5, minmap_pos_0_y + ih * 5,
						minmap_pos_0_x + iw * 5 + 5, minmap_pos_0_y + ih * 5 + 5,
						al_map_rgb(141, 54, 54));
					break;
				case 34:
					al_draw_filled_rectangle(minmap_pos_0_x + iw * 5, minmap_pos_0_y + ih * 5,
						minmap_pos_0_x + iw * 5 + 5, minmap_pos_0_y + ih * 5 + 5,
						al_map_rgb(141, 54, 54));
					break;
				}
			}

		}
		al_draw_rectangle(window_pos_x, window_pos_y, window_pos_x + window_w*5, window_pos_y + window_h*5, al_map_rgb(255, 255, 255), 2);

		al_flip_display();
		al_clear_to_color(al_map_rgb(0, 0, 0));
	}
	 

	al_destroy_bitmap(InterFace);
	al_destroy_event_queue(event_queue);
	al_destroy_display(display);						//destroy our display object

	return 0;
}
