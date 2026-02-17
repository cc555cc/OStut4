#ifndef PLAYERS_H
#define PLAYERS_H

#include <stdbool.h>

#define MAX_PLAYERS 4
#define MAX_NAME_LEN 64

typedef struct {
    char name[MAX_NAME_LEN];
    int score;
    bool is_npc;
} player;

bool player_exists(const player players[], int total_players, const char *name);
void update_score(player players[], int total_players, const char *name, int delta);
int find_player_index(const player players[], int total_players, const char *name);

#endif
