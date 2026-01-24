#include "spynet.h"

void allocate_grid(Game *g) {
    g->grid = (char **)malloc(g->size * sizeof(char *));
    for (int i = 0; i < g->size; i++) {
        g->grid[i] = (char *)malloc(g->size * sizeof(char));
        for (int j = 0; j < g->size; j++) {
            g->grid[i][j] = EMPTY_SYMBOL;
        }
    }
}

void place_items(Game *g) {
    int r, c;

    g->p1.row = rand() % g->size;
    g->p1.col = rand() % g->size;

    do {
        r = rand() % g->size;
        c = rand() % g->size;
    } while (r == g->p1.row && c == g->p1.col);
    g->grid[r][c] = EXTRACT_SYMBOL;

    g->total_intel = 3;
    int placed_intel = 0;
    while (placed_intel < g->total_intel) {
        r = rand() % g->size;
        c = rand() % g->size;
        if ((r != g->p1.row || c != g->p1.col) && g->grid[r][c] == EMPTY_SYMBOL) {
            g->grid[r][c] = INTEL_SYMBOL;
            placed_intel++;
        }
    }

    int placed_lives = 0;
    while (placed_lives < 2) {
        r = rand() % g->size;
        c = rand() % g->size;
        if ((r != g->p1.row || c != g->p1.col) && g->grid[r][c] == EMPTY_SYMBOL) {
            g->grid[r][c] = LIFE_SYMBOL;
            placed_lives++;
        }
    }

    int num_walls = (g->size * g->size) * 0.15;
    int placed_walls = 0;
    while (placed_walls < num_walls) {
        r = rand() % g->size;
        c = rand() % g->size;
        if ((r != g->p1.row || c != g->p1.col) && g->grid[r][c] == EMPTY_SYMBOL) {
            g->grid[r][c] = WALL_SYMBOL;
            placed_walls++;
        }
    }
}

void init_game(Game *g, int size) {
    g->size = size;
    g->game_over = 0;

    g->p1.lives = 3;
    g->p1.intel_collected = 0;
    g->p1.active = 1;

    allocate_grid(g);
    place_items(g);

    log_move(g, '-', "Game Initialized");
}

void display_map(Game *g) {
    printf("\n   ");
    for (int k = 0; k < g->size; k++) printf("---");
    printf("\n");

    for (int i = 0; i < g->size; i++) {
        printf(" | ");
        for (int j = 0; j < g->size; j++) {
            if (i == g->p1.row && j == g->p1.col)
                printf("%c  ", PLAYER_SYMBOL);
            else
                printf("%c  ", g->grid[i][j]);
        }
        printf("|\n");
    }

    printf("   ");
    for (int k = 0; k < g->size; k++) printf("---");
    printf("\n");

    printf("Stats -> Lives: %d | Intel: %d/%d\n",
           g->p1.lives, g->p1.intel_collected, g->total_intel);
}

void log_move(Game *g, char move, const char *message) {
    FILE *fp = fopen("spynet_log.txt", "a");
    if (fp == NULL) return;

    fprintf(fp, "Move: %c | %s\n", move, message);
    fprintf(fp, "Stats: Lives=%d, Intel=%d/%d\n",
            g->p1.lives, g->p1.intel_collected, g->total_intel);

    for (int i = 0; i < g->size; i++) {
        for (int j = 0; j < g->size; j++) {
            if (i == g->p1.row && j == g->p1.col)
                fprintf(fp, "%c ", PLAYER_SYMBOL);
            else
                fprintf(fp, "%c ", g->grid[i][j]);
        }
        fprintf(fp, "\n");
    }
    fprintf(fp, "-----------------------------\n");
    fclose(fp);
}

int is_valid_move(Game *g, int r, int c) {
    if (r < 0 || r >= g->size || c < 0 || c >= g->size)
        return 0;
    if (g->grid[r][c] == WALL_SYMBOL)
        return 0;
    return 1;
}

void process_move(Game *g, char direction) {
    int new_r = g->p1.row;
    int new_c = g->p1.col;
    char msg[100];

    switch (direction) {
        case 'w': case 'W': new_r--; break;
        case 's': case 'S': new_r++; break;
        case 'a': case 'A': new_c--; break;
        case 'd': case 'D': new_c++; break;
        default:
            g->p1.lives--;
            snprintf(msg, 100, "Invalid Input");
            check_game_status(g);
            log_move(g, direction, msg);
            return;
    }

    if (is_valid_move(g, new_r, new_c)) {
        g->p1.row = new_r;
        g->p1.col = new_c;

        char item = g->grid[new_r][new_c];

        if (item == INTEL_SYMBOL) {
            g->p1.intel_collected++;
            g->grid[new_r][new_c] = EMPTY_SYMBOL;
            snprintf(msg, 100, "Collected Intel");
        } else if (item == LIFE_SYMBOL) {
            g->p1.lives++;
            g->grid[new_r][new_c] = EMPTY_SYMBOL;
            snprintf(msg, 100, "Collected Life");
        } else if (item == EXTRACT_SYMBOL) {
            if (g->p1.intel_collected == g->total_intel) {
                g->game_over = 1;
                snprintf(msg, 100, "Mission Complete");
            } else {
                g->p1.active = 0;
                g->game_over = 2;
                snprintf(msg, 100, "Mission Failed");
            }
        } else {
            snprintf(msg, 100, "Moved");
        }
    } else {
        g->p1.lives--;
        snprintf(msg, 100, "Blocked Move");
    }

    check_game_status(g);
    log_move(g, direction, msg);
}

void check_game_status(Game *g) {
    if (g->p1.lives <= 0) {
        g->p1.active = 0;
        g->game_over = 2;
    }
}

void free_memory(Game *g) {
    for (int i = 0; i < g->size; i++)
        free(g->grid[i]);
    free(g->grid);
}
