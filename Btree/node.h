#include<stdbool.h>
#include<stdio.h>

#ifndef NODE_H
#define NODE_H
#define t 128
typedef struct Data{
    int key;
    int page_no;
    int slot;
}Data;
typedef struct BNode{
    bool isLeaf;// if the node is a leaf
    int pos;//the position in file
    int n;//number of keys
    Data records[2*t-1];
    int children[2*t];
}BNode;

#endif

void node_copy(BNode* a,BNode* b);
