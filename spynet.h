#ifndef SPYNET_H
#define SPYNET_H
 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
 
/* Constants */
#define MIN_N 5
#define MAX_N 15
#define MAX_PLAYERS 3
#define INTEL_REQUIRED 3
 
/* Structure Definition */
typedef struct {
    int row, col;
    int lives;
    int intel;
    int active;     
    char symbol;    
} Player;
 
/* Function Prototypes */
char **createGrid(int n);
void freeGrid(char **grid, int n);
void initGrid(char **grid, int n);
void placeItems(char **grid, int n);
void setupPlayers(char **grid, int n, Player players[], int num_players);
void displayGrid(char **grid, int n);
int handleMove(char **grid, int n, Player *p, char move);
void logGameState(FILE *fp, char **grid, int n, Player players[], int num_players, int turnCount);
 
#endif
