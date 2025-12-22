#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "Btree/btree.h"

#ifndef MYUTILS_H
#define MYUTILS_H

#define TOTAL_PAGES 100
#define MAX_NAME_LENGTH 28 


typedef struct row {
    int roll;
    char name[MAX_NAME_LENGTH];
} Row;



#define ROWS_PER_PAGE 127
// #define BITMAP_SIZE (ROWS_PER_PAGE / 16)
#define BITMAP_SIZE ((ROWS_PER_PAGE + 7) / 8)
typedef struct {
    uint8_t bitmap[BITMAP_SIZE];
    Row rows[ROWS_PER_PAGE];
    uint8_t padding[16];    
} Page;

typedef struct {
    Page* pages[TOTAL_PAGES];
    uint8_t active_rows[TOTAL_PAGES];
    FILE* data_pt;
    Btree* btree_index;
} table;


//bitmap functions to manage row deletion flag

int is_active(uint8_t* bm, int i);
void mark_active(uint8_t* bm, int i);
void mark_inactive(uint8_t* bm, int i);
void write_page(FILE* fp, int page_number, Page* page);
Page* read_page(FILE* fp, int page_number);
#endif