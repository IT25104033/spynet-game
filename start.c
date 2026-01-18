#ifndef SPYNET_H
#define SPYNET_H

#include <stdio.h>
#include <stdlib.h>


//game constants 

#define MIN_SIZE 5
#define MAX_SIZE 15

#define PLAYER_SYMBOL 	'@'
#define WALL_SYMBOL 	'#'
#define INTEL_SYMBOL	'I'
#define LIFE_SYMBOL	'L'
#define EXTRACT_SYMBOL	'X'
#define EMPTY_SYMBOL 	'.' //Empty cell
	
typedef struct{
int row;
int col;
int lives;
int intel_collected;
int active; //1 = active , 0= inactive
}Player;

typedef struct{
char **grid	//Dynamically allocated 2D grid
int size	//grid dimention
player p1;
int total_intel;
} Game;

void init_game(Game *g, int size);
void allocate_grid(Game *g);
void place_items(Game *g);

void display_map(struct Game *g);
void log_move(struct Game *g, char move, const char *message);

void process_move(struct Game *g, char direction);
int is_valid_move(struct Game *g, int r, int c);
int check_win_condition(struct Game *g);

void free_memory(struct Game *g);

#endif
