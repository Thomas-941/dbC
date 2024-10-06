#ifndef BTREE_H
#define BTREE_H

#include "table.h"

Node* createNode(Row data);
Node* insert(Node* root, Row data);
Node* search(Node* root, uint32_t id);
Node* delete(Node* root, uint32_t id);

void inorder_traversal(Node* root, void (*callback)(Row*));

#endif