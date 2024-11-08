#ifndef BTREE_H
#define BTREE_H

#include "table.h"

// Enumération des types d'instructions
typedef enum { STATEMENT_INSERT, STATEMENT_SELECT, STATEMENT_DELETE, STATEMENT_USE } StatementType;

// Structure d'une instruction
typedef struct {
    StatementType type;
    Row row_to_insert;
    uint32_t id_to_delete;
    char* where_condition; // Condition WHERE pour les requêtes
    char* where_column;    // Colonne pour la condition WHERE
    char* where_operator;  // Opérateur pour la condition WHERE
    char* where_value;     // Valeur pour la condition WHERE
    char* db_name;         // Nom de la base de données
} Statement;

// Enumération des résultats d'exécution
typedef enum { EXECUTE_SUCCESS, EXECUTE_TABLE_FULL, EXECUTE_DUPLICATE_KEY, EXECUTE_NOT_FOUND, EXECUTE_UNRECOGNIZED_STATEMENT } ExecuteResult;

// Déclarations des fonctions
Node* createNode(Row data);
Node* insert(Node* root, Row data);
Node* search(Node* root, uint32_t id);
Node* delete(Node* root, uint32_t id);

void inorder_traversal(Node* root, void (*callback)(Row*, Statement*), Statement* statement);

// Déclaration des fonctions d'exécution
ExecuteResult execute_insert(Statement* statement, Database* db);
ExecuteResult execute_delete(Statement* statement, Database* db);

#endif