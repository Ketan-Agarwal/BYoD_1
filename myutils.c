#include "myutils.h"

// bitmap functions

int is_active(uint8_t* bm, int i) {
    return bm[i / 8] & (1 << (i % 8));
}
void mark_active(uint8_t* bm, int i) {
    bm[i / 8] |= (1 << (i % 8));
}
void mark_inactive(uint8_t* bm, int i) {
    bm[i / 8] &= ~(1 << (i % 8));
}



// functions to read and write data from table in the disk
void write_page (FILE* fp, int page_number, Page* page) {
    fseek(fp, page_number * sizeof(Page), 0);
    fwrite(page, sizeof(Page), 1, fp);
}

Page* read_page (FILE* fp, int page_number) {
    Page* page = (Page*)malloc(sizeof(Page));
    fseek(fp, page_number*sizeof(Page), 0);
    fread(page, sizeof(page), 1, fp);
    return page;
}
