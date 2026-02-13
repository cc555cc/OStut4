#include "jeopardy.h"
#include "players.h"
#include "questions.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

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
    char *save = NULL;
    int idx = 0;
    for (char *tok = strtok_r(input, delim, &save); tok != NULL; tok = strtok_r(NULL, delim, &save)) {
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

    printf("Welcome to Jeopardy!\n\n");

    for (int i = 0; i < MAX_PLAYERS; i++) {
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
            break;
        }
    }

    initialize_game(questions, TOTAL_QUESTIONS);

    while (remaining_questions(questions, TOTAL_QUESTIONS) > 0) {
        char selector[MAX_NAME_LEN];
        char category[MAX_CATEGORY_TEXT];
        char value_input[INPUT_BUFFER];
        int value = 0;

        display_categories(questions, TOTAL_QUESTIONS);

        while (true) {
            read_line("Who will pick the next question? ", selector, sizeof(selector));
            if (player_exists(players, MAX_PLAYERS, selector)) {
                break;
            }
            printf("Invalid player name. Try again.\n");
        }

        while (true) {
            read_line("Enter category: ", category, sizeof(category));
            read_line("Enter dollar value (100-500): ", value_input, sizeof(value_input));

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

        display_question(questions, TOTAL_QUESTIONS, category, value);

        char answer_input[INPUT_BUFFER];
        char *tokens[MAX_TOKENS] = {0};
        char parsed_answer[MAX_ANSWER_TEXT] = "";
        read_line("Your answer (start with 'what is' or 'who is'): ", answer_input, sizeof(answer_input));
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
            update_score(players, MAX_PLAYERS, selector, value);
        } else {
            printf("Incorrect. The correct answer was: ");
            for (int i = 0; i < TOTAL_QUESTIONS; i++) {
                if (strcasecmp(questions[i].category, category) == 0 && questions[i].value == value) {
                    printf("%s\n", questions[i].answer);
                    break;
                }
            }
        }

        mark_answered(questions, TOTAL_QUESTIONS, category, value);
    }

    show_results(players, MAX_PLAYERS);
    return 0;
}
