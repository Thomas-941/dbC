#ifndef BTREE_C
#define BTREE_C

#include <stdio.h>
#include <stdlib.h>

#include "headers/btree.h"
#include "headers/constants.h"
#include "headers/repl.h"

// Fonction pour créer un nouveau noeud
Node* createNode(Row data) {

    Node* newNode = (Node*)malloc(sizeof(Node));

    if (newNode == NULL) {

        fprintf(stderr, "Erreur : Échec de l'allocation mémoire pour un nouveau nœud.\n");

        exit(EXIT_FAILURE);
        
    }

    newNode->data = data;
    newNode->left = NULL;
    newNode->right = NULL;

    return newNode;

}

// Fonction pour insérer une ligne dans l'arbre binaire
Node* insert(Node* root, Row data) {

    if (root == NULL) {
        return createNode(data);
    }
    
    if (data.id < root->data.id) {
        root->left = insert(root->left, data);
    } else if (data.id > root->data.id) {
        root->right = insert(root->right, data);
    }
    
    return root;

}

// Fonction pour rechercher une ligne par id
Node* search(Node* root, uint32_t id) {

    if (root == NULL || root->data.id == id) {
        return root;
    }
    
    if (id < root->data.id) {
        return search(root->left, id);
    }
    
    return search(root->right, id);

}

// Fonction pour supprimer une ligne par id
Node* delete(Node* root, uint32_t id) {

    if (root == NULL) {
        return root;
    }

    if (id < root->data.id) {
        root->left = delete(root->left, id);
    } else if (id > root->data.id) {
        root->right = delete(root->right, id);
    } else {

        if (root->left == NULL) {

            Node* temp = root->right;

            free(root);

            return temp;

        } else if (root->right == NULL) {

            Node* temp = root->left;

            free(root);

            return temp;

        }

        Node* temp = root->right;

        while (temp->left != NULL) {
            temp = temp->left;
        }

        root->data = temp->data;
        root->right = delete(root->right, temp->data.id);

    }

    return root;

}

// Fonction pour parcourir l'arbre binaire
void inorder_traversal(Node* root, void (*callback)(Row*, Statement*), Statement* statement) {

    if (root != NULL) {

        inorder_traversal(root->left, callback, statement);

        callback(&root->data, statement);

        inorder_traversal(root->right, callback, statement);

    } else {

        return;

    }

}

// Fonction pour exécuter une insertion
ExecuteResult execute_insert(Statement* statement, Database* db) {
    if (db->tables->num_rows >= TABLE_MAX_ROWS) {
        return EXECUTE_TABLE_FULL;
    }

    Row* row_to_insert = &(statement->row_to_insert);
    Node* existing_node = search(db->tables->root, row_to_insert->id);

    if (existing_node != NULL) {
        return EXECUTE_DUPLICATE_KEY;
    }

    db->tables->root = insert(db->tables->root, *row_to_insert);
    db->tables->num_rows += 1;

    save_database(db); // Sauvegarder après l'insertion
    return EXECUTE_SUCCESS;
}

// Fonction pour exécuter une suppression
ExecuteResult execute_delete(Statement* statement, Database* db) {
    if (statement->id_to_delete == 0) {
        return EXECUTE_UNRECOGNIZED_STATEMENT;
    }

    Node* node_to_delete = search(db->tables->root, statement->id_to_delete);

    if (node_to_delete == NULL) {
        return EXECUTE_NOT_FOUND;
    }

    db->tables->root = delete(db->tables->root, statement->id_to_delete);
    db->tables->num_rows -= 1;

    save_database(db); // Sauvegarder après la suppression
    return EXECUTE_SUCCESS;
}

// Fonction pour exécuter une sélection
ExecuteResult execute_select(Statement* statement, Database* db) {
    if (db->tables == NULL) {
        return EXECUTE_NOT_FOUND;
    }

    Node* current = db->tables->root;
    uint32_t id_to_select = statement->row_to_insert.id; // Utiliser l'id de la ligne à insérer comme condition

    Node* result = search(current, id_to_select);
    if (result == NULL) {
        return EXECUTE_NOT_FOUND;
    }

    // Afficher les résultats
    printf("Résultat de la sélection :\n");
    printf("ID: %d, Utilisateur: %s, Email: %s\n", result->data.id, result->data.username, result->data.email);
    
    return EXECUTE_SUCCESS;
}

#endif