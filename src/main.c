/**
 * Graphic/main program
 *
 * Copyright (C) 2023 Saúl Valdelvira
 * License: MIT
 * Last update: 25-03-2022
*/
#define SDL_MAIN_HANDLED 
#include <SDL2/SDL.h>
#include "path_finding.h"
#include <stdio.h>

#ifdef __unix__
    #include <unistd.h>
#elif _WIN32
    #include <windows.h>
#endif

#define CELL_WIDTH 17

int grid_cell_size;
int grid_width;
int grid_height;

Coordinates a_coord;
Coordinates b_coord;

SDL_bool re_draw_path = SDL_FALSE;
SDL_bool show_visited = SDL_FALSE;

Coordinates *coord_to_move;

SDL_Color grid_background;
SDL_Color grid_line_color;
SDL_Color cursor_ghost_color;
SDL_Color point_color;

enum Theme{
	LIGHT, DARK
} theme = DARK;

static void set_theme(){
	if (theme == DARK){
		grid_background = (SDL_Color){22, 22, 22, 255}; // Barely Black
		grid_line_color = (SDL_Color){44, 44, 44, 255}; // Dark grey
		cursor_ghost_color = (SDL_Color){44, 44, 44, 255};
		point_color = (SDL_Color){255, 255, 255, 255}; // White
	}else{
		grid_background = (SDL_Color){233, 233, 233, 255}; // Barely white
		grid_line_color = (SDL_Color){200, 200, 200, 255}; // Very light grey
		cursor_ghost_color = (SDL_Color){200, 200, 200, 255};
		point_color = (SDL_Color){0, 0, 0, 0};
	}
}

void barrier(int x, int y){
	if ((x != a_coord.x || y != a_coord.y) && (x != b_coord.x || y != b_coord.y)){
		put_barrier((Coordinates){x, y});
		re_draw_path = SDL_TRUE;
	}
}

void move_point(int x, int y){
	if (get_barrier((Coordinates){x,y})){
		return;
	}
	if (x == a_coord.x && y == a_coord.y){
		coord_to_move = &a_coord;
	}else if (x == b_coord.x && y == b_coord.y){
		coord_to_move = &b_coord;
	}else {
		coord_to_move->x = x;
		coord_to_move->y = y;
		re_draw_path = SDL_TRUE;
	}
}

void process_key_event(SDL_KeyCode key){
	switch (key){
	case SDLK_m:
		a_coord.x = 0;
		a_coord.y = grid_height - 1;
		b_coord.x = grid_width - 1;
		b_coord.y = 0;
		prepare_maze(a_coord, b_coord);
		re_draw_path = SDL_TRUE;
		break;
	case SDLK_c:
		a_coord.x = (grid_width - 1) / 2;
		a_coord.y = (grid_height - 1) / 2;
		b_coord.x = (grid_width - 1) / 2 - 1;
		b_coord.y = (grid_height - 1) / 2;
		clear_barriers();
		re_draw_path = SDL_TRUE;
		break;
	case SDLK_v:
		show_visited = !show_visited;
		break;
	case SDLK_t:
		theme = theme == DARK ? LIGHT : DARK;
		set_theme();
		break;
	case SDLK_h:
		switch_horizontal_movement();
		re_draw_path = SDL_TRUE;
		break;
	default:
		break;
	}
}

void help(){
	printf(
	"[Path Finding]\n"
	"Ussage: path-finding -d [...] -w [...]\n"
	"Arguments:\n"
	"\t-d <n_rows>x<n_cols> : Set dimensions for the grid\n"
	"\t-w <width> : Set width of grid's cells\n"
	"Keybindings:\n"
	"\t V: Also color the blocks which have been visited during the search.\n"
	"\t M: Fill all the grid with obstacles, so you can draw a maze.\n"
	"\t C: Clear the grid.\n"
	"\t T: Switch between dark and light theme\n"
	"\t H: Toggle horizontal movement on and off\n");
}

int main(int argc, char *argv[]){
	int n_rows = N_ROWS;
	int n_cols = N_COLS;
	grid_cell_size = CELL_WIDTH;
	for (int i = 1; i < argc; i++){
		if (argv[i][0] != '-'){
			continue;
		}
		switch (argv[i][1]){
			case 'd':
				n_rows = atoi(argv[i+1]);
				char *c = argv[i+1];
				while (*c != 'x' && *c != 'X' && *c != '\0'){
					c++;
				}
				if (*c == '\0'){
					fprintf(stderr, "Dimensions must be specified like so: <n_rows>x<n_cols>\n");
					return 1;
				}
				c++;
				n_cols = atoi(c);
				if (n_rows <= 0 || n_cols <= 0){
					fprintf(stderr, "Dimensions must be positive\n");
				}
				i++;
				break;
			case 'w':
				grid_cell_size = atoi(argv[i+1]);
				if (grid_cell_size <= 0){
					grid_cell_size = CELL_WIDTH;
				}
				break;
			case 'h':
				help();
				return 0;
				break;
		}
	}
	grid_width = n_cols;
	grid_height = n_rows;
	if (path_finding_init(n_rows, n_cols) != 1){
		fprintf(stderr, "Error creating the grid at path_finding_init\n");
		return 1;
	}
	Path path = {.path_length = 0};

	int window_width = (grid_width * grid_cell_size) + 1;
	int window_height = (grid_height * grid_cell_size) + 1;

	// Place the points in the middle of the screen.
	SDL_Rect point_a = {
		.x = (grid_width - 1) / 2 * grid_cell_size,
		.y = (grid_height - 1) / 2 * grid_cell_size,
		.w = grid_cell_size,
		.h = grid_cell_size,
	};

	SDL_Rect point_b = {
		.x = point_a.x - 1,
		.y = point_a.y,
		.w = point_a.w,
		.h = point_a.h
	};

	SDL_Rect cursor_ghost = {
		point_a.x,
		point_a.y,
		grid_cell_size,
		grid_cell_size
	};

	SDL_Rect square = {
		point_a.x,
		point_a.y,
		grid_cell_size,
		grid_cell_size
	};

	set_theme();

	SDL_Color grid_barrier_color = {32, 104, 225, 0.8}; // Blue
	SDL_Color grid_path_color = {246, 204, 46, 0.91}; // Yellow
	SDL_Color grid_visited_color = {83, 82, 82, 0.42};

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Initialize SDL: %s", SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_Window *window;
	SDL_Renderer *renderer;
	if (SDL_CreateWindowAndRenderer(window_width, window_height, 0, &window, &renderer) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Create window and renderer: %s", SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_SetWindowTitle(window, "Path Finding");

	SDL_bool quit = SDL_FALSE;
	SDL_bool mouse_active = SDL_FALSE;
	SDL_bool mouse_hover = SDL_FALSE;
	SDL_bool click = SDL_FALSE;

	Coordinates last_clicked;
	int last_button_clicked = 0;

	int x;
	int y;

	a_coord = (Coordinates){.x = point_a.x / grid_cell_size, .y = point_a.y / grid_cell_size};
	b_coord = (Coordinates){.x = point_b.x / grid_cell_size, .y = point_b.y / grid_cell_size};
	coord_to_move = &a_coord;
	while (!quit) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYUP:
				process_key_event(event.key.keysym.sym);
				break;
			case SDL_MOUSEBUTTONDOWN:
				x = event.motion.x / grid_cell_size;
				y = event.motion.y / grid_cell_size;

				if (x < 0 || x >= n_cols || y < 0 || y >= n_rows){
					break;
				}

				click = SDL_TRUE;
				last_clicked.x = x;
				last_clicked.y = y;
				last_button_clicked = event.button.button;

				if (event.button.button == SDL_BUTTON_LEFT){
					move_point(x,y);
				}else if (event.button.button == SDL_BUTTON_RIGHT){
					barrier(x,y);
				}

				break;
			case SDL_MOUSEBUTTONUP:
				click = SDL_FALSE;
				break;
			case SDL_MOUSEMOTION:
				x = event.motion.x / grid_cell_size;
				y = event.motion.y / grid_cell_size;
				if (x < 0 || x >= n_cols || y < 0 || y >= n_rows){
					break;
				}
				cursor_ghost.x = (event.motion.x / grid_cell_size) * grid_cell_size;
				cursor_ghost.y = (event.motion.y / grid_cell_size) * grid_cell_size;

				if (click && (last_clicked.x != x || last_clicked.y != y)){
					if (last_button_clicked == SDL_BUTTON_RIGHT){
						barrier(x, y);
					}else if (last_button_clicked == SDL_BUTTON_LEFT){
						move_point(x,y);
					}
					last_clicked.x = x;
					last_clicked.y = y;
				}

				if (!mouse_active)
					mouse_active = SDL_TRUE;
				break;
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_ENTER && !mouse_hover)
					mouse_hover = SDL_TRUE;
				else if (event.window.event == SDL_WINDOWEVENT_LEAVE && mouse_hover)
					mouse_hover = SDL_FALSE;
				break;
			case SDL_QUIT:
				quit = SDL_TRUE;
				break;
			}
		}

		// Make sure no barrier is set in the two points' coordinates
		if (get_barrier(a_coord)){
			put_barrier(a_coord);
		}
		if (get_barrier(b_coord)){
			put_barrier(b_coord);
		}

		// Draw grid background.
		SDL_SetRenderDrawColor(renderer, grid_background.r, grid_background.g, grid_background.b, grid_background.a);
		SDL_RenderClear(renderer);

		// Draw grid ghost cursor.
		if (mouse_active && mouse_hover) {
			SDL_SetRenderDrawColor(
					renderer,
					cursor_ghost_color.r,
					cursor_ghost_color.g,
					cursor_ghost_color.b,
					cursor_ghost_color.a
					);
			SDL_RenderFillRect(renderer, &cursor_ghost);
		}

		// Draw grid cursor.
		point_a.x = a_coord.x * grid_cell_size;
		point_a.y = a_coord.y * grid_cell_size;
		point_b.x = b_coord.x * grid_cell_size;
		point_b.y = b_coord.y * grid_cell_size;
		SDL_SetRenderDrawColor(
				renderer,
				point_color.r,
				point_color.g,
				point_color.b,
				point_color.a
				);
		SDL_RenderFillRect(renderer, &point_a);
		SDL_RenderFillRect(renderer, &point_b);

		// Draw barriers
		SDL_SetRenderDrawColor(renderer, grid_barrier_color.r, grid_barrier_color.g, grid_barrier_color.b, grid_barrier_color.a);

		for (int i = 0; i < n_rows; i++){
			for (int j = 0; j < n_cols; j++){
				if (get_barrier((Coordinates){j, i})){
					square.x = j * grid_cell_size;
					square.y = i * grid_cell_size;
					SDL_RenderFillRect(renderer, &square);
				}
			}
		}

		// Draw path
		if (re_draw_path){
			path = find_path(a_coord, b_coord);
			re_draw_path = SDL_FALSE;
		}
		if (show_visited){
			SDL_SetRenderDrawColor(renderer, grid_visited_color.r, grid_visited_color.g, grid_visited_color.b, grid_visited_color.a);
			for (int i = 0; i < n_rows; i++){
				for (int j = 0; j < n_cols; j++){
					if (get_visited((Coordinates){j, i}) && !get_barrier((Coordinates){j, i}) && (j != a_coord.x || i != a_coord.y) && (j != b_coord.x || i != b_coord.y)){
						square.x = j * grid_cell_size;
						square.y = i * grid_cell_size;
						SDL_RenderFillRect(renderer, &square);
					}
				}
			}
		}

		SDL_SetRenderDrawColor(renderer, grid_path_color.r, grid_path_color.g, grid_path_color.b, grid_path_color.a);
		for (int i = 1; i < path.path_length-1; i++){
			square.x = path.path[i].x * grid_cell_size;
			square.y = path.path[i].y * grid_cell_size;
			SDL_RenderFillRect(renderer, &square);
		}

		// Draw grid lines.
		SDL_SetRenderDrawColor(
				renderer,
				grid_line_color.r,
				grid_line_color.g,
				grid_line_color.b,
				grid_line_color.a
				);

		for (int x = 0; x < 1 + grid_width * grid_cell_size;
			x += grid_cell_size) {
			SDL_RenderDrawLine(renderer, x, 0, x, window_height);
		}

		for (int y = 0; y < 1 + grid_height * grid_cell_size;
			y += grid_cell_size) {
			SDL_RenderDrawLine(renderer, 0, y, window_width, y);
		}

		SDL_RenderPresent(renderer);

		#ifdef __unix__
		usleep(10000);
		#elif _WIN32
		Sleep(10);
		#endif
	}


	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	path_finding_free();
	return EXIT_SUCCESS;
}
