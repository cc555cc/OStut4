#include "questions.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

const char *CATEGORIES[MAX_CATEGORIES] = {
    "Science",
    "History",
    "Geography",
    "Computers"
};

static void lower_copy(const char *src, char *dst, size_t dst_size) {
    size_t i = 0;
    for (; i + 1 < dst_size && src[i] != '\0'; i++) {
        dst[i] = (char)tolower((unsigned char)src[i]);
    }
    dst[i] = '\0';
}

static bool category_match(const char *a, const char *b) {
    char la[MAX_CATEGORY_TEXT];
    char lb[MAX_CATEGORY_TEXT];
    lower_copy(a, la, sizeof(la));
    lower_copy(b, lb, sizeof(lb));
    return strcmp(la, lb) == 0;
}

void initialize_game(question questions[], int total_questions) {
    (void)total_questions;
    int idx = 0;

    const int values[MAX_QUESTIONS_PER_CAT] = {100, 200, 300, 400, 500};

    const char *science_q[MAX_QUESTIONS_PER_CAT] = {
        "This planet is known as the Red Planet.",
        "H2O is the chemical formula for this.",
        "This gas do plants absorb from the atmosphere?",
        "This organ pumps blood through the body.",
        "The force that keeps us on the ground."
    };
    const char *science_a[MAX_QUESTIONS_PER_CAT] = {
        "mars", "water", "carbon", "heart", "gravity"
    };

    const char *history_q[MAX_QUESTIONS_PER_CAT] = {
        "He was the first President of the United States.",
        "This war was fought between the North and South in the US.",
        "She was the Egyptian queen who allied with Rome.",
        "The ancient city buried by Vesuvius in AD 79.",
        "This document declared American independence."
    };
    const char *history_a[MAX_QUESTIONS_PER_CAT] = {
        "washington", "civil", "cleopatra", "pompeii", "declaration"
    };

    const char *geography_q[MAX_QUESTIONS_PER_CAT] = {
        "This is the largest ocean on Earth.",
        "This river runs through Egypt.",
        "This country is known as the Land of the Rising Sun.",
        "This desert is the largest hot desert.",
        "This mountain range contains Everest."
    };
    const char *geography_a[MAX_QUESTIONS_PER_CAT] = {
        "pacific", "nile", "japan", "sahara", "himalayas"
    };

    const char *computers_q[MAX_QUESTIONS_PER_CAT] = {
        "This device is the brain of the computer.",
        "This acronym stands for Random Access Memory.",
        "This company created the iPhone.",
        "This programming language is known for its snake logo.",
        "This term describes a malicious program."
    };
    const char *computers_a[MAX_QUESTIONS_PER_CAT] = {
        "cpu", "ram", "apple", "python", "virus"
    };

    for (int c = 0; c < MAX_CATEGORIES; c++) {
        for (int v = 0; v < MAX_QUESTIONS_PER_CAT; v++) {
            question *q = &questions[idx++];
            strncpy(q->category, CATEGORIES[c], sizeof(q->category) - 1);
            q->category[sizeof(q->category) - 1] = '\0';

            if (c == 0) {
                strncpy(q->question, science_q[v], sizeof(q->question) - 1);
                q->question[sizeof(q->question) - 1] = '\0';
                strncpy(q->answer, science_a[v], sizeof(q->answer) - 1);
                q->answer[sizeof(q->answer) - 1] = '\0';
            } else if (c == 1) {
                strncpy(q->question, history_q[v], sizeof(q->question) - 1);
                q->question[sizeof(q->question) - 1] = '\0';
                strncpy(q->answer, history_a[v], sizeof(q->answer) - 1);
                q->answer[sizeof(q->answer) - 1] = '\0';
            } else if (c == 2) {
                strncpy(q->question, geography_q[v], sizeof(q->question) - 1);
                q->question[sizeof(q->question) - 1] = '\0';
                strncpy(q->answer, geography_a[v], sizeof(q->answer) - 1);
                q->answer[sizeof(q->answer) - 1] = '\0';
            } else {
                strncpy(q->question, computers_q[v], sizeof(q->question) - 1);
                q->question[sizeof(q->question) - 1] = '\0';
                strncpy(q->answer, computers_a[v], sizeof(q->answer) - 1);
                q->answer[sizeof(q->answer) - 1] = '\0';
            }

            q->value = values[v];
            q->answered = false;
        }
    }
}

void display_categories(const question questions[], int total_questions) {
    (void)total_questions;
    printf("\nAvailable Questions:\n");
    for (int c = 0; c < MAX_CATEGORIES; c++) {
        printf("  %s: ", CATEGORIES[c]);
        for (int v = 0; v < MAX_QUESTIONS_PER_CAT; v++) {
            int value = (v + 1) * 100;
            bool answered = already_answered(questions, MAX_CATEGORIES * MAX_QUESTIONS_PER_CAT, CATEGORIES[c], value);
            if (answered) {
                printf("[---] ");
            } else {
                printf("[$%d] ", value);
            }
        }
        printf("\n");
    }
    printf("\n");
}

void display_question(const question questions[], int total_questions, const char *category, int value, const char *color) {
    for (int i = 0; i < total_questions; i++) {
        if (category_match(questions[i].category, category) && questions[i].value == value) {
            printf("\n%sCategory: %s  Value: $%d\n", color, questions[i].category, value);
            printf("Question: %s\033[0m\n", questions[i].question);
            return;
        }
    }
}

bool valid_answer(const question questions[], int total_questions, const char *category, int value, const char *answer) {
    for (int i = 0; i < total_questions; i++) {
        if (category_match(questions[i].category, category) && questions[i].value == value) {
            char la[MAX_ANSWER_TEXT];
            char lb[MAX_ANSWER_TEXT];
            lower_copy(questions[i].answer, la, sizeof(la));
            lower_copy(answer, lb, sizeof(lb));
            return strcmp(la, lb) == 0;
        }
    }
    return false;
}

bool already_answered(const question questions[], int total_questions, const char *category, int value) {
    for (int i = 0; i < total_questions; i++) {
        if (category_match(questions[i].category, category) && questions[i].value == value) {
            return questions[i].answered;
        }
    }
    return false;
}

void mark_answered(question questions[], int total_questions, const char *category, int value) {
    for (int i = 0; i < total_questions; i++) {
        if (category_match(questions[i].category, category) && questions[i].value == value) {
            questions[i].answered = true;
            return;
        }
    }
}

int remaining_questions(const question questions[], int total_questions) {
    int remaining = 0;
    for (int i = 0; i < total_questions; i++) {
        if (!questions[i].answered) {
            remaining++;
        }
    }
    return remaining;
}
