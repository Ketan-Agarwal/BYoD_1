#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "node.h"

#define ALLOC(x) (x*)malloc(sizeof(x))

#ifndef BTREE_H
#define BTREE_H

typedef struct Btree{
    char fname[20];//filename
    FILE* fp;//pointer to file
    int root;//root node;
    int next_pos;//position of next free space
}Btree;
Btree* BTree_init(char *fname, bool mode);
void insert_non_full(Btree* tree, BNode *node, Data* record);
void insert(Btree* tree, Data* record);
void traverse(Btree* tree, int root);
Data* btree_search(Btree* tree, int key);
Data* search_recursive(Btree* tree, int key, BNode* root);
bool removeFromTree(Btree* tree, int key);
void merge(Btree* tree, BNode *node, int idx);
void borrowFromNext(Btree* tree, BNode *node, int idx);
void borrowFromPrev(Btree* tree, BNode *node, int idx);
void fill(Btree* tree, BNode *node, int idx);
Data *getSucc(Btree* tree, BNode *node, int idx);
Data *getPred(Btree* tree, BNode *node, int idx);
bool removeFromNonLeaf(Btree* tree, BNode *node, int idx);
bool removeFromLeaf (Btree* tree, BNode *node, int idx);
bool removeNode(Btree* tree, BNode* node, int k);
int findKey(BNode* node, int k);
void BTree_destroy(Btree* tree);
#endif