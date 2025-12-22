#include "node.h"

void node_copy(BNode* a,BNode* b){
    a->isLeaf=b->isLeaf;
    a->pos=b->pos;
    a->n=b->n;
    int i;
    for(i=0;i<2*t-1;i++){
        a->records[i]=b->records[i];
        a->children[i]=b->children[i];
    }
    a->children[i]=b->children[i];
}