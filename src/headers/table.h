#ifndef TABLE_H
#define TABLE_H

#include <stdint.h>
#include "constants.h"

typedef struct {

    uint32_t id;
    char username[MAX_USERNAME_LENGTH];
    char email[MAX_EMAIL_LENGTH];

} Row;

typedef struct Node {

    Row data;

    struct Node* left;
    struct Node* right;

} Node;

typedef struct {

    Node* root;
    uint32_t num_rows;

} Table;

Table* new_table();

#endif