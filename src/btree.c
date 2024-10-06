#ifndef BTREE_C
#define BTREE_C

#include <stdio.h>
#include <stdlib.h>

#include "headers/btree.h"
#include "headers/constants.h"

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

Node* search(Node* root, uint32_t id) {

    if (root == NULL || root->data.id == id) {
        return root;
    }
    
    if (id < root->data.id) {
        return search(root->left, id);
    }
    
    return search(root->right, id);

}

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

void inorder_traversal(Node* root, void (*callback)(Row*)) {

    if (root != NULL) {

        inorder_traversal(root->left, callback);

        callback(&root->data);
        
        inorder_traversal(root->right, callback);

    }

}

#endif