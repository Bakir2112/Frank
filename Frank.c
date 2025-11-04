#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define MAX_WORD_COUNT 15000
#define MAX_SUCCESSOR_COUNT (MAX_WORD_COUNT / 2)

char book[] = {
#include "pg84.txt"
, '\0'};

char *tokens[MAX_WORD_COUNT];
size_t tokens_size = 0;

char *succs[MAX_WORD_COUNT][MAX_SUCCESSOR_COUNT];
size_t succs_sizes[MAX_WORD_COUNT];

void replace_non_printable_chars_with_space() {
    for (size_t i = 0; book[i] != '\0'; ++i) {
        if (!isprint((unsigned char)book[i]) && book[i] != '\n' && book[i] != '\r' && book[i] != '\t') {
            book[i] = ' ';
        }
    }
}

size_t token_id(char *token) {
    for (size_t id = 0; id < tokens_size; ++id) {
        if (strcmp(tokens[id], token) == 0) {
            return id;
        }
    }
    tokens[tokens_size] = token;
    return tokens_size++;
}

void append_to_succs(char *token, char *succ) {
    size_t tid = token_id(token);
    size_t *next_empty_index_ptr = &succs_sizes[tid];

    if (*next_empty_index_ptr >= MAX_SUCCESSOR_COUNT) {
        printf("Successor array full.\n");
        exit(EXIT_FAILURE);
    }

    succs[tid][(*next_empty_index_ptr)++] = succ;
}

void tokenize_and_fill_succs(char *delimiters, char *str) {
    char *token = strtok(str, delimiters);
    if (!token) return;
    char *prev = token;
    token_id(prev);

    while ((token = strtok(NULL, delimiters)) != NULL) {
        append_to_succs(prev, token);
        prev = token;
        token_id(prev);
    }
}

char last_char(char *str) {
    size_t len = strlen(str);
    if (len == 0) return '\0';
    return str[len - 1];
}

bool token_ends_a_sentence(char *token) {
    char c = last_char(token);
    return (c == '.' || c == '?' || c == '!');
}

size_t random_token_id_that_starts_a_sentence() {
    size_t id;
    do {
        id = rand() % tokens_size;
    } while (!isupper((unsigned char)tokens[id][0]));
    return id;
}

char *generate_sentence(char *sentence, size_t sentence_size) {
    size_t current_token_id = random_token_id_that_starts_a_sentence();
    char *token = tokens[current_token_id];

    sentence[0] = '\0';
    strcat(sentence, token);
    if (token_ends_a_sentence(token))
        return sentence;

    size_t sentence_len_next = strlen(sentence);
    do {
        size_t num_succ = succs_sizes[current_token_id];
        if (num_succ == 0) break;

        char *next_token = succs[current_token_id][rand() % num_succ];
        sentence_len_next = strlen(sentence) + strlen(next_token) + 2;
        if (sentence_len_next >= sentence_size - 1)
            break;

        strcat(sentence, " ");
        strcat(sentence, next_token);

        current_token_id = token_id(next_token);
    } while (!token_ends_a_sentence(tokens[current_token_id]) &&
             sentence_len_next < sentence_size - 1);

    return sentence;
}

int main() {
    replace_non_printable_chars_with_space();

    char *delimiters = " \n\r";
    tokenize_and_fill_succs(delimiters, book);

    char sentence[1000];
    srand(time(NULL));

    // Generate sentences until we find a question
    do {
        generate_sentence(sentence, sizeof(sentence));
    } while (last_char(sentence) != '?');
    puts(sentence);
    puts("");

    // Generate sentences until we find an exclamation
    do {
        generate_sentence(sentence, sizeof(sentence));
    } while (last_char(sentence) != '!');
    puts(sentence);
}
