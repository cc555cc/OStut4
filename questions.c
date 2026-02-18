#include "questions.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *CATEGORIES[MAX_CATEGORIES] = {
    "Science",
    "History",
    "Geography",
    "Computers"
};

typedef struct {
    const char *category;
    int value;
    const char *question;
    const char *answer;
} question_bank_item;

static const question_bank_item QUESTION_BANK[] = {
    {"Science", 100, "This planet is known as the Red Planet.", "mars"},
    {"Science", 100, "The center of our solar system is this star.", "sun"},
    {"Science", 100, "Water freezes at this temperature in Celsius.", "zero"},
    {"Science", 200, "H2O is the chemical formula for this.", "water"},
    {"Science", 200, "Humans breathe in this gas to survive.", "oxygen"},
    {"Science", 200, "The process by which plants make food.", "photosynthesis"},
    {"Science", 300, "This gas do plants absorb from the atmosphere?", "carbon"},
    {"Science", 300, "This blood type is known as the universal donor.", "o"},
    {"Science", 300, "This part of a cell contains genetic material.", "nucleus"},
    {"Science", 400, "This organ pumps blood through the body.", "heart"},
    {"Science", 400, "The smallest unit of an element is this.", "atom"},
    {"Science", 400, "This force opposes motion between surfaces.", "friction"},
    {"Science", 500, "The force that keeps us on the ground.", "gravity"},
    {"Science", 500, "This scientist proposed the laws of motion.", "newton"},
    {"Science", 500, "DNA stands for deoxyribonucleic this.", "acid"},

    {"History", 100, "He was the first President of the United States.", "washington"},
    {"History", 100, "This wall fell in Berlin in 1989.", "wall"},
    {"History", 100, "This explorer reached the Americas in 1492.", "columbus"},
    {"History", 200, "This war was fought between the North and South in the US.", "civil"},
    {"History", 200, "This ship sank on its first voyage in 1912.", "titanic"},
    {"History", 200, "The U.S. bought this territory from France in 1803.", "louisiana"},
    {"History", 300, "She was the Egyptian queen who allied with Rome.", "cleopatra"},
    {"History", 300, "This Chinese wall stretches thousands of miles.", "great"},
    {"History", 300, "This document begins with We the People.", "constitution"},
    {"History", 400, "The ancient city buried by Vesuvius in AD 79.", "pompeii"},
    {"History", 400, "This empire built roads across much of Europe.", "roman"},
    {"History", 400, "This French leader was defeated at Waterloo.", "napoleon"},
    {"History", 500, "This document declared American independence.", "declaration"},
    {"History", 500, "This movement sought votes for women.", "suffrage"},
    {"History", 500, "This period in Europe is known as the Dark Ages.", "medieval"},

    {"Geography", 100, "This is the largest ocean on Earth.", "pacific"},
    {"Geography", 100, "This continent is home to Egypt.", "africa"},
    {"Geography", 100, "This U.S. state is known as the Sunshine State.", "florida"},
    {"Geography", 200, "This river runs through Egypt.", "nile"},
    {"Geography", 200, "This country is famous for maple syrup.", "canada"},
    {"Geography", 200, "This line divides Earth into north and south hemispheres.", "equator"},
    {"Geography", 300, "This country is known as the Land of the Rising Sun.", "japan"},
    {"Geography", 300, "This city is the capital of Australia.", "canberra"},
    {"Geography", 300, "This sea separates Europe and Africa.", "mediterranean"},
    {"Geography", 400, "This desert is the largest hot desert.", "sahara"},
    {"Geography", 400, "This U.S. river flows to the Gulf of Mexico.", "mississippi"},
    {"Geography", 400, "This mountain is the highest in Africa.", "kilimanjaro"},
    {"Geography", 500, "This mountain range contains Everest.", "himalayas"},
    {"Geography", 500, "This South American rainforest is the largest.", "amazon"},
    {"Geography", 500, "This strait separates Asia from North America.", "bering"},

    {"Computers", 100, "This device is the brain of the computer.", "cpu"},
    {"Computers", 100, "This hardware component displays images.", "monitor"},
    {"Computers", 100, "This key is often used to delete characters.", "backspace"},
    {"Computers", 200, "This acronym stands for Random Access Memory.", "ram"},
    {"Computers", 200, "This input device moves the cursor.", "mouse"},
    {"Computers", 200, "This binary digit is either zero or one.", "bit"},
    {"Computers", 300, "This company created the iPhone.", "apple"},
    {"Computers", 300, "This protocol secures websites with encryption.", "https"},
    {"Computers", 300, "This language styles web pages.", "css"},
    {"Computers", 400, "This programming language is known for its snake logo.", "python"},
    {"Computers", 400, "This structure stores key value pairs in many languages.", "hashmap"},
    {"Computers", 400, "This algorithm technique divides and conquers a problem.", "recursion"},
    {"Computers", 500, "This term describes a malicious program.", "virus"},
    {"Computers", 500, "This Linux command lists files in a directory.", "ls"},
    {"Computers", 500, "This distributed tool is commonly used for version control.", "git"}
};

#define QUESTION_BANK_SIZE ((int)(sizeof(QUESTION_BANK) / sizeof(QUESTION_BANK[0])))

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

    int values[MAX_QUESTIONS_PER_CAT] = {100, 200, 300, 400, 500};
    bool used[QUESTION_BANK_SIZE];
    for (int i = 0; i < QUESTION_BANK_SIZE; i++) {
        used[i] = false;
    }

    int idx = 0;
    for (int c = 0; c < MAX_CATEGORIES; c++) {
        for (int v = 0; v < MAX_QUESTIONS_PER_CAT; v++) {
            int candidates[QUESTION_BANK_SIZE];
            int candidate_count = 0;
            int desired_value = values[v];

            for (int i = 0; i < QUESTION_BANK_SIZE; i++) {
                if (!used[i] &&
                    strcmp(QUESTION_BANK[i].category, CATEGORIES[c]) == 0 &&
                    QUESTION_BANK[i].value == desired_value) {
                    candidates[candidate_count++] = i;
                }
            }

            question *q = &questions[idx++];
            strncpy(q->category, CATEGORIES[c], sizeof(q->category) - 1);
            q->category[sizeof(q->category) - 1] = '\0';
            q->value = desired_value;
            q->answered = false;

            if (candidate_count == 0) {
                strncpy(q->question, "No question available.", sizeof(q->question) - 1);
                q->question[sizeof(q->question) - 1] = '\0';
                strncpy(q->answer, "unknown", sizeof(q->answer) - 1);
                q->answer[sizeof(q->answer) - 1] = '\0';
                continue;
            }

            int chosen_idx = candidates[rand() % candidate_count];
            used[chosen_idx] = true;

            strncpy(q->question, QUESTION_BANK[chosen_idx].question, sizeof(q->question) - 1);
            q->question[sizeof(q->question) - 1] = '\0';
            strncpy(q->answer, QUESTION_BANK[chosen_idx].answer, sizeof(q->answer) - 1);
            q->answer[sizeof(q->answer) - 1] = '\0';
        }
    }
}

void display_categories(const question questions[], int total_questions) {
    printf("\nAvailable Questions:\n");
    for (int c = 0; c < MAX_CATEGORIES; c++) {
        printf("  %s: ", CATEGORIES[c]);
        for (int v = 0; v < MAX_QUESTIONS_PER_CAT; v++) {
            int value = (v + 1) * 100;
            bool answered = already_answered(questions, total_questions, CATEGORIES[c], value);
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

void display_question(const question questions[], int total_questions, const char *category, int value) {
    for (int i = 0; i < total_questions; i++) {
        if (category_match(questions[i].category, category) && questions[i].value == value) {
            printf("\nCategory: %s  Value: $%d\n", questions[i].category, value);
            printf("Question: %s\n", questions[i].question);
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

void get_random_unanswered(const question questions[], int total_questions, char *category_out, int *value_out) {
    int unanswered[MAX_CATEGORIES * MAX_QUESTIONS_PER_CAT];
    int count = 0;

    for (int i = 0; i < total_questions; i++) {
        if (!questions[i].answered) {
            unanswered[count++] = i;
        }
    }

    if (count > 0) {
        int idx = unanswered[rand() % count];
        strncpy(category_out, questions[idx].category, MAX_CATEGORY_TEXT - 1);
        category_out[MAX_CATEGORY_TEXT - 1] = '\0';
        *value_out = questions[idx].value;
    }
}
