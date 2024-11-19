#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <dirent.h>

#include "headers/btree.h"
#include "headers/table.h"
#include "headers/constants.h"
#include "headers/repl.h"

// Fonction pour créer une ligne de test
Row create_test_row(uint32_t id, const char* username, const char* email) {

    Row row;
    row.id = id;

    strncpy(row.username, username, MAX_USERNAME_LENGTH);
    strncpy(row.email, email, MAX_EMAIL_LENGTH);

    return row;

}

// Fonction pour libérer les noeuds en mémoire
void free_nodes(Node* root) {

    if (root != NULL) {

        free_nodes(root->left);
        free_nodes(root->right);
        free(root);
        
    }

}

// Fonction pour exécuter tous les tests
int main() {

    printf("Exécution des tests unitaires...\n");
    
    // Tests pour les fonctions
    test_createNode();
    test_insert_and_search();
    test_delete();
    test_inorder_traversal();
    test_add_column();
    test_remove_column();
    test_modify_column();
    test_add_table();
    test_add_table_with_columns();
    test_repl_functions();
    test_select();
    test_show_tables_and_columns();
    test_do_meta_command();
    test_database_management();
    
    printf("Tous les tests ont réussi !\n");

    return 0;

}