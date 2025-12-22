#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<stdbool.h>
#include "myutils.h"

#ifndef CACHE_H
#define CACHE_H

#define CACHE_SIZE 25 // Maximum number of pages in the cache

typedef struct CacheNode {
    int page_number; 
    Page* page;
    struct CacheNode* next;
    bool changed;
} CacheNode;

CacheNode* insert_page_in_cache(int page_number, Page* page, FILE* file);
Page* get_page_from_cache(int page_number);
CacheNode* find_cache_node(int page_number);
void flush_cache(FILE* data_pt);
#endif