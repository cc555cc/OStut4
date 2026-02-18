#include "jeopardy.h"
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
#define INPUT_BUFFER MAX_LEN

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

void tokenize(char *input, char **tokens) {
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

void show_results(player players[], int total_players) {
    player sorted[MAX_PLAYERS];
    for (int i = 0; i < total_players; i++) {
        sorted[i] = players[i];
    }

    for (int i = 0; i < total_players - 1; i++) {
        for (int j = 0; j < total_players - 1 - i; j++) {
            if (sorted[j].score < sorted[j + 1].score) {
                player tmp = sorted[j];
                sorted[j] = sorted[j + 1];
                sorted[j + 1] = tmp;
            }
        }
    }

    printf("\nFinal Results:\n");
    for (int i = 0; i < total_players; i++) {
        printf("%d. %s - $%d\n", i + 1, sorted[i].name, sorted[i].score);
    }
    printf("\nWinner: %s\n", sorted[0].name);
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
        char *end = NULL;
        long n = strtol(num_input, &end, 10);
        if (end != num_input && *end == '\0' && n >= 1 && n <= total_players) {
            num_human_players = (int)n;
            break;
        }
        printf("Invalid number. Please enter 1-%d.\n", total_players);
    }

    for (int i = 0; i < num_human_players; i++) {
        char name[MAX_NAME_LEN];
        while (true) {
            char prompt[64];
            snprintf(prompt, sizeof(prompt), "Enter name for Player %d: ", i + 1);
            read_line(prompt, name, sizeof(name));
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
            break;
        }
    }

    for (int i = num_human_players; i < total_players; i++) {
        snprintf(players[i].name, sizeof(players[i].name), "NPC %d", i - num_human_players + 1);
        players[i].score = 0;
        players[i].is_npc = true;
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
            printf("Invalid player name. Try again.\n");
        }

        if (quit_game) {
            printf("\nGame terminated early.\n");
            break;
        }

        bool is_npc_turn = players[current_player_idx].is_npc;

        if (is_npc_turn) {
            get_random_unanswered(questions, TOTAL_QUESTIONS, category, &value);
            printf("%s picks: %s for $%d\n", selector, category, value);
        } else {
            while (true) {
                read_line("Enter category: ", category, sizeof(category));
                if (is_quit_command(category)) {
                    quit_game = true;
                    break;
                }
                read_line("Enter dollar value (100-500): ", value_input, sizeof(value_input));
                if (is_quit_command(value_input)) {
                    quit_game = true;
                    break;
                }

                if (!parse_value(value_input, &value)) {
                    printf("Invalid dollar value. Try again.\n");
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
                    printf("Unknown category. Try again.\n");
                    continue;
                }

                if (already_answered(questions, TOTAL_QUESTIONS, category, value)) {
                    printf("That question has already been answered. Choose another.\n");
                    continue;
                }
                break;
            }
        }

        if (quit_game) {
            printf("\nGame terminated early.\n");
            break;
        }

        display_question(questions, TOTAL_QUESTIONS, category, value);

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
                printf("%s answers: what is %s\n", selector, parsed_answer);
                printf("Correct! +$%d\n", value);
                update_score(players, total_players, selector, value);
            } else {
                printf("%s answers: what is unknown\n", selector);
                printf("Incorrect. The correct answer was: ");
                for (int i = 0; i < TOTAL_QUESTIONS; i++) {
                    if (strcasecmp(questions[i].category, category) == 0 && questions[i].value == value) {
                        printf("%s\n", questions[i].answer);
                        break;
                    }
                }
            }
        } else {
            read_line("Your answer (start with 'what is' or 'who is'): ", answer_input, sizeof(answer_input));
            
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
                printf("Invalid answer format. No points awarded.\n");
            } else if (valid_answer(questions, TOTAL_QUESTIONS, category, value, parsed_answer)) {
                printf("Correct! +$%d\n", value);
                update_score(players, total_players, selector, value);
            } else {
                printf("Incorrect. The correct answer was: ");
                for (int i = 0; i < TOTAL_QUESTIONS; i++) {
                    if (strcasecmp(questions[i].category, category) == 0 && questions[i].value == value) {
                        printf("%s\n", questions[i].answer);
                        break;
                    }
                }
            }
        }

        mark_answered(questions, TOTAL_QUESTIONS, category, value);
    }

    show_results(players, total_players);
    return 0;
}
