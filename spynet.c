#include "spynet.h"
 
void displayGrid(char **grid, int n) {
    printf("\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%c ", grid[i][j]);
        }
        printf("\n");
    }
}
 
void logGameState(FILE *fp, char **grid, int n, Player players[], int num_players, int turnCount) {
    fprintf(fp, "RECORD #%d\n", turnCount);
    for (int i = 0; i < num_players; i++) {
        fprintf(fp, "P%c: %s | L:%d I:%d\n", 
                players[i].symbol, players[i].active ? "ACTIVE" : "OUT",
                players[i].lives, players[i].intel);
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) fprintf(fp, "%c ", grid[i][j]);
        fprintf(fp, "\n");
    }
    fprintf(fp, "--------------------------------\n\n");
    fflush(fp);
}
 
char **createGrid(int n) {
    char **g = malloc(n * sizeof(char *));
    for (int i = 0; i < n; i++) g[i] = malloc(n * sizeof(char));
    return g;
}
 
void freeGrid(char **grid, int n) {
    for (int i = 0; i < n; i++) free(grid[i]);
    free(grid);
}
 
void initGrid(char **grid, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) grid[i][j] = '.';
}
 
void placeItems(char **grid, int n) {
    char items[] = {'#', 'I', 'L', 'X'};
    int counts[] = {n, 3, 2, 1}; 
    for (int i = 0; i < 4; i++) {
        int placed = 0;
        while (placed < counts[i]) {
            int r = rand() % n, c = rand() % n;
            if (grid[r][c] == '.' && !(r==0 && c==0) && !(r==n-1 && c==n-1) && !(r==0 && c==n-1)) {
                grid[r][c] = items[i];
                placed++;
            }
        }
    }
}
 
void setupPlayers(char **grid, int n, Player players[], int num_players) {
    for (int i = 0; i < num_players; i++) grid[players[i].row][players[i].col] = players[i].symbol;
}
 
int handleMove(char **grid, int n, Player *p, char move) {
    int nr = p->row, nc = p->col;
    if (move == 'W' || move == 'w') nr--;
    else if (move == 'S' || move == 's') nr++;
    else if (move == 'A' || move == 'a') nc--;
    else if (move == 'D' || move == 'd') nc++;
    else return 1;
 
    if (nr < 0 || nr >= n || nc < 0 || nc >= n || grid[nr][nc] == '#') return 0;
 
    char target = grid[nr][nc];
    if (target == 'I') p->intel++;
    else if (target == 'L') p->lives++;
    else if (target == 'X') return (p->intel >= INTEL_REQUIRED) ? 2 : 3;
 
    grid[p->row][p->col] = '.';
    p->row = nr; p->col = nc;
    grid[nr][nc] = p->symbol;
    return 1;
}
