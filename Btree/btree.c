#include "btree.h"

Btree* BTree_init(char* fname,bool mode){
    Btree* tree=ALLOC(Btree);
    if(!mode){
        strcpy(tree->fname,fname);
        tree->fp=fopen(fname,"wb+");
        tree->root=-1;
        tree->next_pos=0;
        return tree;
    }
    else{
        FILE* fin=fopen(fname, "rb");
        if (fin) {
            // File exists read the tree structure
            fread(tree, sizeof(Btree), 1, fin);
            fclose(fin);
            tree->fp=fopen(fname, "rb+");
            return tree;
        } else {
            // File doesnt exist create new one
            strcpy(tree->fname, fname);
            tree->fp=fopen(fname, "wb+");
            tree->root=-1;
            tree->next_pos=0;
            return tree;
        }
    }
}
void BTree_destroy(Btree* tree) {
    if (tree) {
        if (tree->fp) {
            fclose(tree->fp);  
        }
        free(tree);
    }
}
void splitChild(Btree* tree, BNode* x, int i, BNode* y) {
    BNode* z = malloc(sizeof(BNode)); node_init(z, y->isLeaf, tree);
    z->n = t - 1;
    
    int j;
    for(j = 0; j < t-1; j++) {
        z->records[j] = y->records[j+t];// copy last t-1 records from y to z
    }
    
    if(!y->isLeaf) {
        for(j = 0; j < t; j++) {
            z->children[j] = y->children[j+t];// copy the children too
        }
    }
    
    y->n = t - 1;// keep the first t-1 keys
    
    for(j = x->n; j >= i+1; j--) {
        x->children[j+1] = x->children[j];// make space for adding new children
    }
    
    x->children[i+1] = z->pos;// add the child there
    
    for(j = x->n-1; j >= i; j--) {
        x->records[j+1] = x->records[j];//shift the records too
    }
    
    x->records[i] = y->records[t - 1];// now place the promoted record in specified postion
    
    x->n++;// increase the key vae
    
    
    write_file(tree, x, x->pos);// write all these new datas in the file
    write_file(tree, y, y->pos);
    write_file(tree, z, z->pos);
    free(z);
    
}
void insert_non_full(Btree* tree, BNode *node, Data *record) {
    
    int i = (node->n)-1;//start from last value
    
    if(node->isLeaf) {
        
        while(i >=0 && node->records[i].key > record->key) {
            node->records[i+1] = node->records[i];// shift accoding to the key value
            i--;
        }
        node->records[i+1] = *record;
        node->n++;
        write_file(tree, node, node->pos);
        
    }
    
    else {
        while( i >= 0 && node->records[i].key > record->key) {
            i--;// go to the appropriate node
        }
        
        BNode *c_i = (BNode*)malloc(sizeof(BNode));//initialze a new node and read the value from the child to this node
        
        read_file(tree, c_i, node->children[i+1]);
        
        if(c_i->n == (2*t-1)) {
            splitChild(tree, node, i+1, c_i);// with node as parent and c_i as child split it
            
            if(node->records[i+1].key < record->key) {
                i++;//move to the appropriate node
            }
        }
        read_file(tree, c_i, node->children[i+1]);// go to the appropriate children
        insert_non_full(tree, c_i, record);// insert in this node
        free(c_i);// free the temp node as we have already write_file in insert_npn full
    }
    
}
void insert(Btree* tree, Data *record) {
    
    if(tree->root == -1) {  // First insertion
        tree->root = tree->next_pos;
        BNode *root_node = malloc(sizeof(BNode)); 
        node_init(root_node, true, tree);
        root_node->records[0] = *record;
        root_node->n = 1;
        write_file(tree, root_node, root_node->pos);
        free(root_node);
    }

    else{
        if(!(tree->next_pos)) {// if we are adding the first node in our tree
            tree->root = tree->next_pos;
            BNode *root_node = malloc(sizeof(BNode)); node_init(root_node, true, tree);
            root_node->records[0] = *record;//add the data
            root_node->n++;// n=1 from n=0
            write_file(tree, root_node, root_node->pos);// write the data and exit
            free(root_node);
        }
        
        else {
            BNode *root = malloc(sizeof(BNode));
            read_file(tree, root, tree->root);// copy everything from root of the tree to this root node
            if(root->n == (2*t-1)) {
                
                BNode *new_root = malloc(sizeof(BNode)); node_init(new_root, false, tree);// c
                new_root->children[0] = tree->root;
                
                splitChild(tree, new_root, 0, root);// with new root as parent and root as child split the child
                
                int i = 0;
                if(new_root->records[0].key < record->key) {
                    i++;// move to the appropriate spot
                }
                
                BNode *c_i = (BNode*)malloc(sizeof(BNode));
                read_file(tree, c_i, new_root->children[i]);// copy the content of the child to this node
                insert_non_full(tree, c_i, record);// 
                
                tree->root = new_root->pos;
                
                write_file(tree, root, root->pos);// write in the file
                write_file(tree,new_root,new_root->pos);
                free(new_root);
            }
            
            else {
                
                insert_non_full(tree, root, record);
                
            }
            
            free(root);
        }
    }
}
Data* search_recursive(Btree* tree, int key, BNode* root) {
    int i = 0;
    
    while(i < root->n && key > root->records[i].key)
        i++;// move to the appropriate location
    
    
    if(i < root->n && key == root->records[i].key){
        Data* result = malloc(sizeof(Data));
        *result = root->records[i];
        return result;//  great if you found the key at this node , else check if the given node is leaf else go to the approprotate child
    }
    
    if(root->isLeaf) {
        return NULL;
    }
    BNode* c_i = malloc(sizeof(BNode));
    read_file(tree, c_i, root->children[i]);
    Data* result = search_recursive(tree, key, c_i);
    free(c_i);
    return result;
}

Data* btree_search(Btree* tree, int key) {
    
    if (tree->root == -1) {
        return NULL;  // Tree is empty
    }

    BNode* root = (BNode*)malloc(sizeof(BNode));
    read_file(tree, root, tree->root);
    Data* result = search_recursive(tree, key, root);
    free(root);
    
    return result;
     
}
int findKey(BNode* node, int k) {
    int idx=0;
    while (idx < node->n && node->records[idx].key < k)
        ++idx;
    return idx;
}
bool removeNode(Btree* tree, BNode* node, int k) {
    int idx = findKey(node, k);
    bool res;
    
    if (idx < node->n && node->records[idx].key == k) {        
        if (node->isLeaf)
            res = removeFromLeaf(tree, node, idx);
        else
            res = removeFromNonLeaf(tree, node, idx);
        
        write_file(tree, node, node->pos);//update the tree
    }
    else {
        if (node->isLeaf) {
            printf("Not Present!\n");
            return false;
        }
        bool flag = ( (idx==node->n)? true : false );    
        BNode *c_i = (BNode*)malloc(sizeof(BNode));
        read_file(tree, c_i, node->children[idx]);
        
        if (c_i->n < t)
            fill(tree, node, idx);// ensuring that after deletion number of keys dont go below minimum
        if (flag && idx >= node->n) {// ig its wrong it should be idx<node->n
            BNode *sibling = (BNode*)malloc(sizeof(BNode));
            read_file(tree, sibling, node->children[idx-1]);
            removeNode(tree, sibling, k);
            write_file(tree, sibling, sibling->pos);
            if(sibling) free(sibling);
        }
        else
            removeNode(tree, c_i, k);
        
        write_file(tree, c_i, c_i->pos);
        res = true;
        free(c_i);
        // if(sibling) free(sibling);
    }
    return res;
}
bool removeFromLeaf (Btree* tree, BNode *node, int idx) {
    for (int i=idx+1; i<node->n; ++i)
        node->records[i-1] = node->records[i];// shift back by one place equivalently deleting the record at idx
    node->n--;
 
    return true;
}
 bool removeFromNonLeaf(Btree* tree, BNode *node, int idx) {
 
    int k = node->records[idx].key;
    bool res;
    
    BNode *child = (BNode*)malloc(sizeof(BNode));
    BNode *sibling = (BNode*)malloc(sizeof(BNode));
    
    read_file(tree, child, node->children[idx]);// get the child and its previous sibling(it should be next sibling though the right child)
    read_file(tree, sibling, node->children[idx+1]);
    if (child->n >= t) {
        Data *pred = getPred(tree, node, idx);// get the max value from leaves from left tree
        node->records[idx] = *pred;//set the key at that idx with the founded max value from leaves from left tree
        res = removeNode(tree, child, pred);// recusively delete that pred
    }
    else if  (sibling->n >= t)
    {
        Data *succ = getSucc(tree, node, idx);
        node->records[idx] = *succ;
        res = removeNode(tree, sibling, succ);// similar to what is done for left child
    }
    else {
        merge(tree, node, idx);// merge the two children and take the idx key to this merget child and then call the function recursively
        res = removeNode(tree, child, k);
    }
    write_file(tree, child, child->pos);
    write_file(tree, sibling, sibling->pos);
    
    return res;
}
 Data *getPred(Btree* tree, BNode *node, int idx) {
    
    
    BNode *curr = (BNode*)malloc(sizeof(BNode));
    read_file(tree, curr, node->children[idx]);
    while (!curr->isLeaf){
        // read_file(tree, curr, curr->children[curr->n]);
    int child_idx=curr->n;
    int next_pos=curr->children[child_idx];
    free(curr);
    curr=(BNode*)malloc(sizeof(BNode));
    read_file(tree,curr,next_pos);
    }   
    Data* result=(Data*)malloc(sizeof(Data));
    *result=curr->records[curr->n-1];
    free(curr);
    return result;
}
 
Data *getSucc(Btree* tree, BNode *node, int idx) {
 
    
    BNode *curr = (BNode*)malloc(sizeof(BNode));
    read_file(tree, curr, node->children[idx]);
    while (!curr->isLeaf){
        // read_file(tree, curr, curr->children[0]);
    int child_idx=0;
    int next_pos=curr->children[child_idx];
    free(curr);
    curr=(BNode*)malloc(sizeof(BNode));
    read_file(tree,curr,next_pos);
    }
     Data* result=(Data*)malloc(sizeof(Data));
    *result=curr->records[0];
    free(curr);
    return result;// get the lowest key from the bottom in the right tree
}
void fill(Btree* tree, BNode *node, int idx) {
 
    BNode *c_prev = (BNode*)malloc(sizeof(BNode));
    BNode *c_succ = (BNode*)malloc(sizeof(BNode));
    read_file(tree, c_prev, node->children[idx-1]);
    read_file(tree, c_succ, node->children[idx+1]);
    
    
    if (idx!=0 && c_prev->n>=t)
        borrowFromPrev(tree, node, idx);
    else if (idx!=node->n && c_succ->n>=t)
        borrowFromNext(tree, node, idx);
    else {// if neither left nor right sibling has >=t keys then marge them
        if (idx != node->n)
            merge(tree, node, idx);
        else
            merge(tree, node, idx-1);
    }
    return;
}
void borrowFromPrev(Btree* tree, BNode *node, int idx) {
    BNode *child = (BNode*)malloc(sizeof(BNode));
    BNode *sibling = (BNode*)malloc(sizeof(BNode));
    
    read_file(tree, child, node->children[idx]);
    read_file(tree, sibling, node->children[idx-1]);
    for (int i=child->n-1; i>=0; --i)
        child->records[i+1] = child->records[i];// make space for borrowing
 
    
    if (!child->isLeaf) {
        for(int i=child->n; i>=0; --i)
            child->children[i+1] = child->children[i];//shiftb the children accordingly
    }
 
    
    child->records[0] = node->records[idx-1];// move from parent key to child
 
    
    if (!child->isLeaf)// what? obv node isnot a leaf why checking it?
        child->children[0] = sibling->children[sibling->n];//move the children too
 
    node->records[idx-1] = sibling->records[sibling->n-1];// move the last key of sibling to node 
 
    child->n += 1;
    sibling->n -= 1;
    
    write_file(tree, node, node->pos);
    write_file(tree, child, child->pos);
    write_file(tree, sibling, sibling->pos);
    
    return;
}
 


void borrowFromNext(Btree* tree, BNode *node, int idx) {
 
    BNode *child = (BNode*)malloc(sizeof(BNode));
    BNode *sibling = (BNode*)malloc(sizeof(BNode));
    
    read_file(tree, child, node->children[idx]);
    read_file(tree, sibling, node->children[idx+1]);
 
    
    child->records[(child->n)] = node->records[idx];
 
    
    
    if (!(child->isLeaf))
        child->children[(child->n)+1] = sibling->children[0];
 
    
    node->records[idx] = sibling->records[0];
 
    
    for (int i=1; i<sibling->n; ++i)
        sibling->records[i-1] = sibling->records[i];
 
    
    if (!sibling->isLeaf) {// why are we checking this? we already shifted sibling->children[0]? huh?
        for(int i=1; i<=sibling->n; ++i)
            sibling->children[i-1] = sibling->children[i];
    }
 
    
    
    child->n += 1;
    sibling->n -= 1;
    
    write_file(tree, node, node->pos);
    write_file(tree, child, child->pos);
    write_file(tree, sibling, sibling->pos);
    
    return;
}
void merge(Btree* tree, BNode *node, int idx) {
    
    BNode *child = (BNode*)malloc(sizeof(BNode));
    BNode *sibling = (BNode*)malloc(sizeof(BNode));
    
    read_file(tree, child, node->children[idx]);
    read_file(tree, sibling, node->children[idx+1]);
 
    
    
    child->records[t-1] = node->records[idx];//send the mid key from left child to parent
 
    
    for (int i=0; i<sibling->n; ++i)
        child->records[i+t] = sibling->records[i];
 
    
    if (!child->isLeaf) {
        for(int i=0; i<=sibling->n; ++i)
            child->children[i+t] = sibling->children[i];// copy thee values from sibling to child
    }
 
    
    
    for (int i=idx+1; i<node->n; ++i)
        node->records[i-1] = node->records[i];//adjust the records of parent
 
    
    
    for (int i=idx+2; i<=node->n; ++i)
        node->children[i-1] = node->children[i];//adjust pointets to children
 
    
    child->n += sibling->n+1;//+1 because taking from aprent
    node->n--;
 
    
    write_file(tree, node, node->pos);
    write_file(tree, child, child->pos);
    write_file(tree, sibling, sibling->pos);
    return;
}
bool removeFromTree(Btree* tree, int key) {
    
    BNode *root = (BNode*)malloc(sizeof(BNode));
    read_file(tree, root, tree->root);
    return removeNode(tree, root, key);
    
}