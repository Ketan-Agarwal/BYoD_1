#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "node.h"
#include "btree.h"

void write_file(Btree* ptr,BNode* p,int pos);
void read_file(Btree* ptr,BNode* p,int pos);

// void print_fn(Data* node);
void node_init(BNode* node,bool isleaf, Btree* tree);
Data* get_data(char* file,int len);
