#include <stdio.h>
#include <stdlib.h>
#include "headers/repl.h"
#include "headers/table.h"

void free_nodes(Node* node) {

    if (node == NULL) {
        return;
    }

    free_nodes(node->left);
    free_nodes(node->right);
    free(node);

}

void free_table(Table* table) {

    if (table == NULL) {
        return;
    }

    free_nodes(table->root);
    free(table);

}

int main(void/*int argc, char* argv[]*/) {

    Table* table = new_table();

    if (table == NULL) {

        fprintf(stderr, "Erreur: Impossible de créer une nouvelle table.\n");

        return 1;

    }

    repl(table);

    free_table(table);

    return 0;
    
}