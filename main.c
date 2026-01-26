#include "spynet.h"
 
int main() {
    int n, num_players, turn = 0, gameRunning = 1, turnCount = 1;
    char moveInput;
    FILE *logFile;
 
    printf("--- SpyNet: The Codebreaker Protocol ---\n");
    
    printf("Enter grid size (%d-%d): ", MIN_N, MAX_N);
    if (scanf("%d", &n) != 1 || n < MIN_N || n > MAX_N) return 1;
 
    printf("Select Game Mode (1, 2, or 3 players): ");
    if (scanf("%d", &num_players) != 1 || num_players < 1 || num_players > MAX_PLAYERS) return 1;
 
    srand((unsigned int)time(NULL));
    char **grid = createGrid(n);
    initGrid(grid, n);
    placeItems(grid, n);
 
    Player players[MAX_PLAYERS] = {
        {0, 0, 3, 0, 1, '@'},           
        {n - 1, n - 1, 3, 0, 1, '&'},   
        {0, n - 1, 3, 0, 1, '$'}        
    };
    setupPlayers(grid, n, players, num_players);
 
    logFile = fopen("game_log.txt", "w");
    if (logFile == NULL) return 1;
 
    while (gameRunning) {
        Player *p = &players[turn];
 
        if (p->active) {
            displayGrid(grid, n);
            printf("Player %c | Lives: %d | Intel: %d/%d\n", 
                   p->symbol, p->lives, p->intel, INTEL_REQUIRED);
            printf("Move (WASD) or Q to quit: ");
            
            // 1. Read the first character
            scanf(" %c", &moveInput); 
            
            // 2. Clear the buffer (prevents "cheating" or accidental multi-moves)
            int c;
            while ((c = getchar()) != '\n' && c != EOF); 
 
            if (moveInput == 'Q' || moveInput == 'q') {
                grid[p->row][p->col] = '.';
                p->active = 0;
            } else {
                int res = handleMove(grid, n, p, moveInput);
                if (res == 0) {
                    p->lives--;
                    if (p->lives <= 0) {
                        grid[p->row][p->col] = '.';
                        p->active = 0;
                    }
                } else if (res == 2) {
                    printf("\nMission Success: Player %c extracted!\n", p->symbol);
                    gameRunning = 0;
                } else if (res == 3) {
                    printf("\nMission Failure: Player %c captured at extraction.\n", p->symbol);
                    grid[p->row][p->col] = '.';
                    p->active = 0;
                }
            }
            logGameState(logFile, grid, n, players, num_players, turnCount++);
        }
 
        int activeCount = 0, lastIdx = -1;
        for (int i = 0; i < num_players; i++) {
            if (players[i].active) { activeCount++; lastIdx = i; }
        }
 
        if (activeCount == 0 && gameRunning) {
            printf("\nGame Over: All agents neutralized.\n");
            gameRunning = 0;
        } else if (activeCount == 1 && num_players > 1 && gameRunning) {
            printf("\nVictory: Player %c is the sole survivor!\n", players[lastIdx].symbol);
            gameRunning = 0;
        }
 
        turn = (turn + 1) % num_players;
    }
 
    fclose(logFile);
    freeGrid(grid, n);
    return 0;
}
