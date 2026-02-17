#include "players.h"
#include <ctype.h>
#include <string.h>

static void lower_copy(const char *src, char *dst, size_t dst_size) {
    size_t i = 0;
    for (; i + 1 < dst_size && src[i] != '\0'; i++) {
        dst[i] = (char)tolower((unsigned char)src[i]);
    }
    dst[i] = '\0';
}

static bool name_match(const char *a, const char *b) {
    char la[MAX_NAME_LEN];
    char lb[MAX_NAME_LEN];
    lower_copy(a, la, sizeof(la));
    lower_copy(b, lb, sizeof(lb));
    return strcmp(la, lb) == 0;
}

bool player_exists(const player players[], int total_players, const char *name) {
    return find_player_index(players, total_players, name) >= 0;
}

int find_player_index(const player players[], int total_players, const char *name) {
    for (int i = 0; i < total_players; i++) {
        if (name_match(players[i].name, name)) {
            return i;
        }
    }
    return -1;
}

void update_score(player players[], int total_players, const char *name, int delta) {
    int idx = find_player_index(players, total_players, name);
    if (idx >= 0) {
        players[idx].score += delta;
    }
}

void set_player_color(player *p, const char *color) {
    p->color = color;
}

const char *get_player_color(const player *p){
    return p -> color;
}
