#include "butils.h"

void node_init(BNode *node, bool isLeaf, Btree *tree) {
    
    node->isLeaf = isLeaf;// tell us whther new node is a leaf
    node->n = 0;// no key vale pairs
    node->pos = tree->next_pos;//as node occupies the next free space available for tree
    tree->next_pos++;// increase it
    
    for(int i = 0; i < 2*t; i++) {
        node->children[i] = -1;// no children
    }   
}
void write_file(Btree* tree_ptr,BNode* p,int pos){
    if(pos==-1){
        pos=tree_ptr->next_pos++;
    }
    fseek(tree_ptr->fp,pos*sizeof(BNode),0);
    fwrite(p,sizeof(BNode),1,tree_ptr->fp);
}
void read_file(Btree* ptr_tree,BNode* p,int pos){
    fseek(ptr_tree->fp,pos*sizeof(BNode),0);
    fread(p,sizeof(BNode),1,ptr_tree->fp);
}
