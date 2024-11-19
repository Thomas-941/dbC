#include <stdio.h>
#include <stdlib.h>
#include "headers/repl.h"
#include "headers/table.h"

// Fonction pour libérer la mémoire des noeuds de l'arbre binaire
void free_nodes(Node* node) {

    if (node == NULL) {
        return;
    }

    free_nodes(node->left);
    free_nodes(node->right);
    free(node);

}

// Fonction pour libérer la mémoire d'une table
void free_table(Table* table) {

    if (table == NULL) {
        return;
    }

    free_nodes(table->root);
    free(table);

}

// Fonction pour libérer la mémoire d'une base de données
void free_database(Database* db) {

    if (db == NULL) {
        return;
    }

    Table* current_table = db->tables;
    while (current_table != NULL) {
        Table* next_table = current_table->next;
        free_table(current_table);
        current_table = next_table;
    }

    free(db);

}

int main(void) {

    // Initialisation de la base de données à Null
    Database* db = NULL;

    repl(db); // Passez la base de données à la fonction repl

    free_database(db); // Libération de la mémoire

    return 0;
    
}