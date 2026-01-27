#include "spynet.h"
 
int main() {
    int n, num_players, turn = 0, gameRunning = 1, turnCount = 1;
    int is_bot[MAX_PLAYERS] = {0, 0, 0}; 
    char moveInput;
    FILE *logFile;

    //setup and menu
    printf("--- SpyNet: The Codebreaker Protocol ---\n");
    
    printf("Enter grid size (%d-%d): ", MIN_N, MAX_N);
    if (scanf("%d", &n) != 1 || n < MIN_N || n > MAX_N) return 1;
 
    printf("Select Number of Players (1, 2, or 3): ");
    if (scanf("%d", &num_players) != 1 || num_players < 1 || num_players > MAX_PLAYERS) return 1;
 
    if (num_players > 1) {
        printf("Select Mode:\n1. Human vs Human\n2. Human vs Computer\nEnter choice: ");
        int modeChoice;
        scanf("%d", &modeChoice);
        if (modeChoice == 2) {
            for (int i = 1; i < num_players; i++) is_bot[i] = 1;
        }
    }

    //start grid
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

    //main game loop
    while (gameRunning) {
        Player *p = &players[turn];
 
        if (p->active) {
            displayGrid(grid, n, players, num_players);
            printf("\n>>> YOUR TURN: Agent %c %s\n", p->symbol, is_bot[turn] ? "(AI)" : "");
            
            int res;
            if (is_bot[turn]) {
                printf("AI calculating move...\n");
                res = handleComputerMove(grid, n, p);
            } else {
                printf("Enter move (W/A/S/D) or Q to quit: ");
                scanf(" %c", &moveInput);
                int c; while ((c = getchar()) != '\n' && c != EOF); //clear input buffer
 
                if (moveInput == 'Q' || moveInput == 'q') {
                    grid[p->row][p->col] = '.';
                    p->active = 0;
                    res = 1;
                } else {
                    res = handleMove(grid, n, p, moveInput);
                }
            }
 
            //results
            if (res == 0) {
                p->lives--;
                printf("CRITICAL: Collision detected! Lives remaining: %d\n", p->lives);
                if (p->lives <= 0) { 
                    printf("Agent %c has been neutralized.\n", p->symbol);
                    grid[p->row][p->col] = '.'; 
                    p->active = 0; 
                }
            } else if (res == 2) {
                displayGrid(grid, n, players, num_players);
                printf("\nSUCCESS: Agent %c extracted successfully!\n", p->symbol);
                gameRunning = 0;
            } else if (res == 3) {
                printf("\nFAILURE: Agent %c attempted extraction without Intel.\n", p->symbol);
                grid[p->row][p->col] = '.'; 
                p->active = 0;
            }
            logGameState(logFile, grid, n, players, num_players, turnCount++);
        }
 
        
        int activeCount = 0;
        int lastIdx = -1;
        for (int i = 0; i < num_players; i++) {
            if (players[i].active) { activeCount++; lastIdx = i; }
        }
 
        if (activeCount == 0 && gameRunning) {
            printf("\nGAME OVER: No active agents remaining.\n");
            gameRunning = 0;
        } else if (num_players > 1 && activeCount == 1 && gameRunning) {
            printf("\nVICTORY: Agent %c is the last one on the field!\n", players[lastIdx].symbol);
            gameRunning = 0;
        }
 
        turn = (turn + 1) % num_players;
    }
 
    fclose(logFile);
    freeGrid(grid, n);
    return 0;
}

