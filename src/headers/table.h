#ifndef TABLE_H
#define TABLE_H

#include <stdint.h>
#include "constants.h"

// Structure d'une ligne dans la table
typedef struct {

    uint32_t id;
    char username[MAX_USERNAME_LENGTH];
    char email[MAX_EMAIL_LENGTH];

} Row;

// Structure d'un noeud dans l'arbre binaire
typedef struct Node {

    Row data;

    struct Node* left;
    struct Node* right;

} Node;

// Structure d'une colonne dans la table
typedef struct Column {
    char name[MAX_USERNAME_LENGTH];
    char type[MAX_USERNAME_LENGTH];
    struct Column* next;
} Column;

// Structure d'une table
typedef struct Table {
    char name[MAX_USERNAME_LENGTH];
    Node* root;
    uint32_t num_rows;
    Column* columns;
    struct Table* next;
} Table;

// Structure d'une base de données
typedef struct Database {
    char name[MAX_DB_NAME_LENGTH];
    Table* tables;
} Database;

// Déclarations des fonctions pour la gestion des tables
Table* new_table();
void add_column(Table* table, const char* column_name, const char* column_type);
void remove_column(Table* table, const char* column_name);
void modify_column(Table* table, const char* column_name, const char* new_type);
void load_table(Table* table, const char* path);
void load_database(Database* db);
Database* create_database(const char* db_name);
void save_table(Table* table);
void save_database(Database* db);
void delete_table(Database* db, const char* table_name);

#endif