#ifndef QUESTIONS_H
#define QUESTIONS_H

#include <stdbool.h>

#define MAX_CATEGORIES 4
#define MAX_QUESTIONS_PER_CAT 5
#define MAX_QUESTION_TEXT 256
#define MAX_ANSWER_TEXT 64
#define MAX_CATEGORY_TEXT 64

typedef struct {
    char category[MAX_CATEGORY_TEXT];
    char question[MAX_QUESTION_TEXT];
    char answer[MAX_ANSWER_TEXT];
    int value;
    bool answered;
} question;

extern const char *CATEGORIES[MAX_CATEGORIES];

void initialize_game(question questions[], int total_questions);
void display_categories(const question questions[], int total_questions);
void display_question(const question questions[], int total_questions, const char *category, int value);
bool valid_answer(const question questions[], int total_questions, const char *category, int value, const char *answer);
bool already_answered(const question questions[], int total_questions, const char *category, int value);
void mark_answered(question questions[], int total_questions, const char *category, int value);
int remaining_questions(const question questions[], int total_questions);

#endif
