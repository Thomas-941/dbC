#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "headers/btree.h"
#include "headers/table.h"
#include "headers/constants.h"
#include "headers/repl.h"

void print_row_if_condition(Row* row, const char* condition);

typedef struct {

  char* buffer;
  size_t buffer_length;
  ssize_t input_length;

} InputBuffer;

typedef enum { PREPARE_SUCCESS, PREPARE_UNRECOGNIZED_STATEMENT, PREPARE_SYNTAX_ERROR } PrepareResult;

typedef enum { STATEMENT_INSERT, STATEMENT_SELECT, STATEMENT_DELETE } StatementType;

typedef struct {

  StatementType type;
  Row row_to_insert;
  uint32_t id_to_delete;
  char* where_condition;

} Statement;

PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement);

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

void print_row_wrapper(Row* row);

// Test pour la fonction createNode
void test_createNode() {

    Row test_row = create_test_row(1, "test_user", "test@test.fr");
    Node* node = createNode(test_row);
    
    assert(node != NULL);
    assert(node->data.id == 1);
    assert(strcmp(node->data.username, "test_user") == 0);
    assert(strcmp(node->data.email, "test@test.fr") == 0);
    assert(node->left == NULL);
    assert(node->right == NULL);
    
    free(node);

    printf("Test createNode réussi.\n");

}

// Test pour les fonctions insert et search
void test_insert_and_search() {

    Node* root = NULL;

    Row row1 = create_test_row(2, "user2", "user2@test.fr");
    Row row2 = create_test_row(1, "user1", "user1@test.fr");
    Row row3 = create_test_row(3, "user3", "user3@test.fr");
    
    root = insert(root, row1);
    root = insert(root, row2);
    root = insert(root, row3);
    
    Node* found = search(root, 2);

    assert(found != NULL);
    assert(found->data.id == 2);
    assert(strcmp(found->data.username, "user2") == 0);
    
    found = search(root, 1);  
    assert(found != NULL);
    assert(found->data.id == 1);
    
    found = search(root, 4);
    assert(found == NULL);
    
    // Libérer la mémoire
    free_nodes(root);
    
    printf("Test insert et search réussi.\n");
    
}

// Test pour la fonction delete
void test_delete() {
    
    Node* root = NULL;

    Row row1 = create_test_row(2, "user2", "user2@test.fr");
    Row row2 = create_test_row(1, "user1", "user1@test.fr");
    Row row3 = create_test_row(3, "user3", "user3@test.fr");
    
    root = insert(root, row1);
    root = insert(root, row2);
    root = insert(root, row3);
    
    root = delete(root, 2);
    
    assert(search(root, 2) == NULL);
    assert(search(root, 1) != NULL);
    assert(search(root, 3) != NULL);
    
    // Libérer la mémoire
    free_nodes(root);
    
    printf("Test delete réussi.\n");

}

// Fonction wrapper pour print_row_if_condition
void print_row_wrapper(Row* row) {
    print_row_if_condition(row, NULL);
}

// Test pour la fonction inorder_traversal
void test_inorder_traversal() {

    Node* root = NULL;

    Row row1 = create_test_row(2, "user2", "user2@test.fr");
    Row row2 = create_test_row(1, "user1", "user1@test.fr");
    Row row3 = create_test_row(3, "user3", "user3@test.fr");
    
    root = insert(root, row1);
    root = insert(root, row2);
    root = insert(root, row3);
    
    printf("Test inorder_traversal:\n");
    inorder_traversal(root, print_row_wrapper);
    
    free_nodes(root);
    
    printf("Test inorder_traversal réussi.\n");
}

// Test pour les fonctions du REPL
void test_repl_functions() {

    InputBuffer input_buffer = {.buffer = "insert 1 user1 user1@test.fr", .buffer_length = 0, .input_length = 0};
    Statement statement;
    
    PrepareResult result = prepare_statement(&input_buffer, &statement);

    assert(result == PREPARE_SUCCESS);
    assert(statement.type == STATEMENT_INSERT);
    assert(statement.row_to_insert.id == 1);
    assert(strcmp(statement.row_to_insert.username, "user1") == 0);
    printf("Email attendu: user1@test.fr, Email obtenu: %s\n", statement.row_to_insert.email);
    assert(strncmp(statement.row_to_insert.email, "user1@test.fr", MAX_EMAIL_LENGTH) == 0);
    
    printf("Test des fonctions du REPL réussi.\n");
    
}

// Test pour la fonction select
void test_select() {

    Node* root = NULL;

    Row row1 = create_test_row(2, "user2", "user2@test.fr");
    Row row2 = create_test_row(1, "user1", "user1@test.fr");
    Row row3 = create_test_row(3, "user3", "user3@test.fr");
    
    root = insert(root, row1);
    root = insert(root, row2);
    root = insert(root, row3);
    
    printf("Test select:\n");
    printf("Résultat attendu: user1, user2, user3\n");
    inorder_traversal(root, print_row_wrapper);
    
    free_nodes(root);
    
    printf("Test select réussi.\n");
}

// Fonction principale pour exécuter tous les tests
int main() {

    printf("Exécution des tests unitaires...\n");
    
    test_createNode();
    test_insert_and_search();
    test_delete();
    test_inorder_traversal();
    test_repl_functions();
    test_select();
    
    printf("Tous les tests ont réussi !\n");

    return 0;

}