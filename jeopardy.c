#include "players.h"
#include "questions.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>

#define TOTAL_QUESTIONS (MAX_CATEGORIES * MAX_QUESTIONS_PER_CAT)

#define INPUT_BUFFER 256
#define MAX_TOKENS 10

#define DOUBLE_POINTS_CHANCE 20

static int g_questions_taken[MAX_PLAYERS] = {0};

static void trim_newline(char *s) {
    size_t len = strlen(s);
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r')) {
        s[len - 1] = '\0';
        len--;
    }
}

static void to_lower_inplace(char *s) {
    for (; *s != '\0'; s++) {
        *s = (char)tolower((unsigned char)*s);
    }
}

static void read_line(const char *prompt, char *buffer, size_t buffer_size) {
    printf("%s", prompt);
    if (fgets(buffer, (int)buffer_size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    trim_newline(buffer);
}

static bool is_quit_command(const char *input) {
    char tmp[INPUT_BUFFER];
    strncpy(tmp, input, sizeof(tmp) - 1);
    tmp[sizeof(tmp) - 1] = '\0';
    to_lower_inplace(tmp);
    return strcmp(tmp, "quit") == 0 || strcmp(tmp, "exit") == 0;
}

static bool parse_value(const char *input, int *value) {
    char *end = NULL;
    long v = strtol(input, &end, 10);
    if (end == input || *end != '\0') {
        return false;
    }
    if (v != 100 && v != 200 && v != 300 && v != 400 && v != 500) {
        return false;
    }
    *value = (int)v;
    return true;
}

static int roll_multiplier(void) {
    int r = rand() % 100;
    return (r < DOUBLE_POINTS_CHANCE) ? 2 : 1;
}

static void display_scoreboard(const player players[], int total_players) {
    printf("\n=== Scoreboard ===\n");
    for (int i = 0; i < total_players; i++) {
        const char *c = get_player_color(&players[i]);
        if (c == NULL) {
            c = "\033[0m";
        }
        printf("%s%s\033[0m: $%d (Questions used: %d)%s\n",
               c,
               players[i].name,
               players[i].score,
               g_questions_taken[i],
               players[i].is_npc ? " [NPC]" : "");
    }
    printf("==================\n\n");
}

static void tokenize(char *input, char **tokens) {
    for (size_t i = 0; input[i] != '\0'; i++) {
        if (ispunct((unsigned char)input[i])) {
            input[i] = ' ';
        }
    }

    to_lower_inplace(input);

    const char *delim = " \t";
    int idx = 0;
    for (char *tok = strtok(input, delim); tok != NULL; tok = strtok(NULL, delim)) {
        if (idx >= MAX_TOKENS - 1) {
            break;
        }
        tokens[idx++] = tok;
    }
    tokens[idx] = NULL;
}

static void show_results(player players[], int total_players) {
    player sorted[MAX_PLAYERS];
    int qt_sorted[MAX_PLAYERS];

    for (int i = 0; i < total_players; i++) {
        sorted[i] = players[i];
        qt_sorted[i] = g_questions_taken[i];
    }

    for (int i = 0; i < total_players - 1; i++) {
        for (int j = 0; j < total_players - 1 - i; j++) {
            bool swap = false;

            if (sorted[j].score < sorted[j + 1].score) {
                swap = true;
            } else if (sorted[j].score == sorted[j + 1].score) {
                if (qt_sorted[j] > qt_sorted[j + 1]) {
                    swap = true;
                }
            }

            if (swap) {
                player tmp = sorted[j];
                sorted[j] = sorted[j + 1];
                sorted[j + 1] = tmp;

                int t = qt_sorted[j];
                qt_sorted[j] = qt_sorted[j + 1];
                qt_sorted[j + 1] = t;
            }
        }
    }

    printf("\nFinal Results:\n");
    for (int i = 0; i < total_players; i++) {
        printf("%d. %s - $%d (Questions used: %d)\n",
               i + 1, sorted[i].name, sorted[i].score, qt_sorted[i]);
    }

    if (total_players >= 2 &&
        sorted[0].score == sorted[1].score &&
        qt_sorted[0] == qt_sorted[1]) {
        printf("\nResult: TRUE TIE (same score and same questions used)\n");
    } else {
        printf("\nWinner: %s\n", sorted[0].name);
        if (total_players >= 2 && sorted[0].score == sorted[1].score) {
            printf("(Tie-breaker applied: fewer questions used)\n");
        }
    }
}

static const char *assign_color(int index) {
    switch (index) {
        case 0: return "\033[33m";        // yellow
        case 1: return "\033[38;5;208m";  // orange
        case 2: return "\033[34m";        // blue
        case 3: return "\033[35m";        // purple
        default: return "\033[0m";
    }
}

int main(void) {
    player players[MAX_PLAYERS];
    question questions[TOTAL_QUESTIONS];

    int total_players = 0;
    int num_human_players = 0;

    srand((unsigned int)time(NULL));

    printf("Welcome to Jeopardy!\n\n");

    char num_input[INPUT_BUFFER];

    while (true) {
        read_line("How many total players (1-4)? ", num_input, sizeof(num_input));
        if (is_quit_command(num_input)) {
            printf("Goodbye.\n");
            return 0;
        }

        char *end = NULL;
        long n = strtol(num_input, &end, 10);
        if (end != num_input && *end == '\0' && n >= 1 && n <= MAX_PLAYERS) {
            total_players = (int)n;
            break;
        }
        printf("Invalid number. Please enter 1-4.\n");
    }

    while (true) {
        char prompt[64];
        snprintf(prompt, sizeof(prompt), "How many human players (1-%d)? ", total_players);
        read_line(prompt, num_input, sizeof(num_input));
        if (is_quit_command(num_input)) {
            printf("Goodbye.\n");
            return 0;
        }

        char *end = NULL;
        long n = strtol(num_input, &end, 10);
        if (end != num_input && *end == '\0' && n >= 1 && n <= total_players) {
            num_human_players = (int)n;
            break;
        }
        printf("Invalid number. Please enter 1-%d.\n", total_players);
    }

    for (int i = 0; i < MAX_PLAYERS; i++) {
        g_questions_taken[i] = 0;
    }

    for (int i = 0; i < num_human_players; i++) {
        char name[MAX_NAME_LEN];
        const char *color = assign_color(i);

        while (true) {
            char prompt[96];
            snprintf(prompt, sizeof(prompt), "%sEnter name for Player %d: \033[0m", color, i + 1);
            read_line(prompt, name, sizeof(name));

            if (is_quit_command(name)) {
                printf("Goodbye.\n");
                return 0;
            }

            if (name[0] == '\0') {
                printf("Name cannot be empty. Try again.\n");
                continue;
            }
            if (player_exists(players, i, name)) {
                printf("That name is already taken. Try again.\n");
                continue;
            }

            strncpy(players[i].name, name, sizeof(players[i].name) - 1);
            players[i].name[sizeof(players[i].name) - 1] = '\0';
            players[i].score = 0;
            players[i].is_npc = false;
            set_player_color(&players[i], color);
            break;
        }
    }

    for (int i = num_human_players; i < total_players; i++) {
        snprintf(players[i].name, sizeof(players[i].name), "NPC %d", i - num_human_players + 1);
        players[i].score = 0;
        players[i].is_npc = true;
        set_player_color(&players[i], assign_color(i));
    }

    initialize_game(questions, TOTAL_QUESTIONS);

    printf("\nType 'quit' or 'exit' at any prompt to end the game early.\n");

    bool quit_game = false;

    while (!quit_game && remaining_questions(questions, TOTAL_QUESTIONS) > 0) {
        char selector[MAX_NAME_LEN];
        char category[MAX_CATEGORY_TEXT];
        char value_input[INPUT_BUFFER];
        int value = 0;

        display_categories(questions, TOTAL_QUESTIONS);

        int current_player_idx = -1;
        while (true) {
            read_line("Who will pick the next question? ", selector, sizeof(selector));
            if (is_quit_command(selector)) {
                quit_game = true;
                break;
            }

            current_player_idx = find_player_index(players, total_players, selector);
            if (current_player_idx >= 0) {
                break;
            }
            printf("\033[31mInvalid player name. Try again.\033[0m\n");
        }

        if (quit_game) {
            printf("\nGame terminated early.\n");
            break;
        }

        bool is_npc_turn = players[current_player_idx].is_npc;
        const char *color = get_player_color(&players[current_player_idx]);
        if (color == NULL) {
            color = "\033[0m";
        }

        if (is_npc_turn) {
            get_random_unanswered(questions, TOTAL_QUESTIONS, category, &value);
            printf("%s%s\033[0m picks: %s for $%d\n", color, selector, category, value);
        } else {
            while (true) {
                char prompt[160];

                snprintf(prompt, sizeof(prompt), "%sEnter category: \033[0m", color);
                read_line(prompt, category, sizeof(category));
                if (is_quit_command(category)) {
                    quit_game = true;
                    break;
                }

                snprintf(prompt, sizeof(prompt), "%sEnter dollar value (100-500): \033[0m", color);
                read_line(prompt, value_input, sizeof(value_input));
                if (is_quit_command(value_input)) {
                    quit_game = true;
                    break;
                }

                if (!parse_value(value_input, &value)) {
                    printf("\033[31mInvalid dollar value. Try again.\033[0m\n");
                    continue;
                }

                bool found = false;
                for (int c = 0; c < MAX_CATEGORIES; c++) {
                    if (strcasecmp(category, CATEGORIES[c]) == 0) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    printf("\033[31mUnknown category. Try again.\033[0m\n");
                    continue;
                }

                if (already_answered(questions, TOTAL_QUESTIONS, category, value)) {
                    printf("\033[3mThat question has already been answered. Choose another.\033[0m\n");
                    continue;
                }

                break;
            }
        }

        if (quit_game) {
            printf("\nGame terminated early.\n");
            break;
        }

        // Track usage + double points
        g_questions_taken[current_player_idx]++;

        int multiplier = roll_multiplier();
        if (multiplier == 2) {
            printf(">>> DOUBLE POINTS ACTIVE! ($%d -> $%d)\n", value, value * 2);
        }

        // IMPORTANT: 5-arg version with color (matches your questions.h)
        display_question(questions, TOTAL_QUESTIONS, category, value, color);

        char answer_input[INPUT_BUFFER];
        char *tokens[MAX_TOKENS] = {0};
        char parsed_answer[MAX_ANSWER_TEXT] = "";

        if (is_npc_turn) {
            int correct_chance = rand() % 100;
            bool npc_correct = correct_chance < 60;

            if (npc_correct) {
                for (int i = 0; i < TOTAL_QUESTIONS; i++) {
                    if (strcasecmp(questions[i].category, category) == 0 && questions[i].value == value) {
                        strncpy(parsed_answer, questions[i].answer, sizeof(parsed_answer) - 1);
                        parsed_answer[sizeof(parsed_answer) - 1] = '\0';
                        break;
                    }
                }

                printf("%s%s\033[0m answers: what is %s\n", color, selector, parsed_answer);

                int points = value * multiplier;
                printf("\033[32mCorrect! +$%d\033[0m\n", points);
                update_score(players, total_players, selector, points);
            } else {
                printf("%s%s\033[0m answers: what is unknown\n", color, selector);
                printf("\033[31mIncorrect. The correct answer was: \033[0m");
                for (int i = 0; i < TOTAL_QUESTIONS; i++) {
                    if (strcasecmp(questions[i].category, category) == 0 && questions[i].value == value) {
                        printf("%s\n", questions[i].answer);
                        break;
                    }
                }
            }
        } else {
            char prompt[180];
            snprintf(prompt, sizeof(prompt),
                     "%sYour answer (start with 'what is' or 'who is'): \033[0m", color);
            read_line(prompt, answer_input, sizeof(answer_input));

            if (is_quit_command(answer_input)) {
                printf("\nGame terminated early.\n");
                quit_game = true;
                break;
            }

            tokenize(answer_input, tokens);

            if (tokens[0] && tokens[1] && tokens[2] &&
                (strcmp(tokens[0], "what") == 0 || strcmp(tokens[0], "who") == 0) &&
                strcmp(tokens[1], "is") == 0) {
                strncpy(parsed_answer, tokens[2], sizeof(parsed_answer) - 1);
                parsed_answer[sizeof(parsed_answer) - 1] = '\0';
            }

            if (parsed_answer[0] == '\0') {
                printf("\033[31mInvalid answer format. No points awarded.\033[0m\n");
            } else if (valid_answer(questions, TOTAL_QUESTIONS, category, value, parsed_answer)) {
                int points = value * multiplier;
                printf("\033[32mCorrect! +$%d\033[0m\n", points);
                update_score(players, total_players, selector, points);
            } else {
                printf("\033[31mIncorrect. The correct answer was: \033[0m");
                for (int i = 0; i < TOTAL_QUESTIONS; i++) {
                    if (strcasecmp(questions[i].category, category) == 0 && questions[i].value == value) {
                        printf("%s\n", questions[i].answer);
                        break;
                    }
                }
            }
        }

        mark_answered(questions, TOTAL_QUESTIONS, category, value);
        display_scoreboard(players, total_players);
    }

    show_results(players, total_players);
    return 0;
}
