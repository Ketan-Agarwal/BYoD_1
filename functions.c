#include "functions.h"
#include "Btree/btree.h"
#include <limits.h>

int valid_roll(char* input){
    int value=0;
    for(int i=0; input[i]!='\0'; i++){
        if(input[i]<'0' || input[i]>'9') {
            printf("Invalid roll number. Please enter a positive integer.\n");
            return -1; // Indicate invalid input
        }
        if(value > (INT_MAX - (input[i] - '0')) / 10) {
            printf("Roll number too large. Please enter a  roll number in range of 0 to %d.\n", INT_MAX);
            return -1;
        }
        value = value * 10 + (input[i] - '0'); // Convert character to integer
    }
    return value; // Return the valid roll number
}

bool adjust_name(char* name, size_t max_len) {
    if (strchr(name, '\n') == NULL) {
        // Overflow occurred, clear the input buffer
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);
        return false; // Indicate overflow
    }
    // Remove newline character from name if present
    size_t len = strlen(name);
    if(len>=max_len){
        printf("Name exceeds maximum length of %zu characters.\n", max_len);
        return false; // Indicate overflow if name exceeds max length
    }
    if (len > 0 && name[len - 1] == '\n') {
        name[len - 1] = '\0';
        len--;
    }
    if (name[len] != '\0') {
        name[len] = '\0'; // Ensure the name is null-terminated
    }
    return true; // No overflow
}


Page* create_page() { // Function to create a new page
    // Allocate memory for a new page
    Page* new_page = (Page*)malloc(sizeof(Page));
    if (!new_page) {
        printf("Not enough Memory for a page =[ .\n");
        // Print relevant information if memory allocation fails
        return NULL;
    }
    for (int i = 0; i < ROWS_PER_PAGE; i++) { // Initialize all rows in the page
        new_page->rows[i].name[0] = '\0'; // Initialize the name
        new_page->rows[i].roll = 0; // Initialize the roll number
    }
    for (int i = 0; i < BITMAP_SIZE; i++){
        new_page->bitmap[i] = 0;
    }
    return new_page;
}

table* create_table() { // Function to create a new table

    // Allocate memory for a new table
    table* new_table = (table*)malloc(sizeof(table));
    if (!new_table) {
        printf("Not enough Memory for a table =[ .\n");
        return NULL; // If memory allocation fails, print an error message and return NULL
    }  

    // initialize memory fields
    for (int i = 0; i < TOTAL_PAGES; i++) {
        new_table->pages[i] = NULL; // No page loaded initially
        new_table->active_rows[i] = 0;
    }

    // open/create a table file
    new_table->data_pt = fopen("table.db", "rb+");
    
    // create if doesn't exist
    if (!new_table->data_pt) {
        new_table->data_pt = fopen("table.db", "wb+");
        if (!new_table->data_pt) {
            perror("Failed to create/open table.db\n");
            free(new_table);
            return NULL;
        }
    }

    // a fresh B-tree
    new_table->btree_index = BTree_init("index.db", false);
    if (!new_table->btree_index) {
        fclose(new_table->data_pt);
        free(new_table);
        return NULL;
    }
    
    // and rebuild B-tree index from existing data
    printf("Rebuilding B-tree index...\n");
    for (int i = 0; i < TOTAL_PAGES; i++) {
        Page* page = get_page(new_table, i);
        if (!page) continue;
        
        for (int j = 0; j < ROWS_PER_PAGE; j++) {
            if (is_active(page->bitmap, j)) {
                Data* new_data = (Data*)malloc(sizeof(Data));
                new_data->key = page->rows[j].roll;
                new_data->page_no = i;
                new_data->slot = j;
                insert(new_table->btree_index, new_data);
                free(new_data);
            }
        }
    }
    
    return new_table;
}

bool search_table(table* table, int roll){
    
    Data* found = btree_search(table->btree_index, roll);
    
    if (found) {
        Page* page = get_page(table, found->page_no);
        if (page && is_active(page->bitmap, found->slot)) {
            Row* row = &page->rows[found->slot];
            printf("Page Number: %d\nRow Number: %d\nRoll: %d\nName: %s\n",
                   found->page_no, found->slot, row->roll, row->name);
            free(found);
            return true;
        }
        free(found);
    }
    return false;

}

// function to print all rows in table
void print_table(table* table){

    bool found = false; 
    for (int i = 0; i < TOTAL_PAGES; i++){// iterate through all pages
        // Page* page = table->pages[i];
        Page* page = get_page(table, i);
        if (!page) continue;//check if page is null or has no active rows

        uint8_t* bm = page->bitmap;

        for (int j = 0; j < ROWS_PER_PAGE; j++){
            
            // check if row contains data or not
            if (is_active(bm, j)) {
                found = true;
                Row* row = &page->rows[j];
                printf("Page Number: %d, Row Number: %d, Roll: %d, Name: %s\n", i, j, row->roll, row->name);
            }
        }
    }

    if (!found) printf("Table is empty.\n");
    return;
}

bool insert_row(table* table, int roll, const char* name){

    for (int i = 0; i < TOTAL_PAGES; i++) {// Traverse through all pages
        Page* page = get_page(table, i);
        if(!page){// If the page is NULL, it means it has not been allocated yet
            page = create_page(); // Create a new page
            if (!page) return false; // If page creation fails, return false
            insert_page_in_cache(i, page, table->data_pt); // insert the newly created page in cache
            table->pages[i] = page; // Assign the newly created page to the table
        }
        else if (table->active_rows[i] == ROWS_PER_PAGE) continue; // If the page is full, skip to the next page
        
        uint8_t* bm = page->bitmap;// Get the bitmap array for the current page, where each bit represents the status of a row
        for (int j = 0; j < ROWS_PER_PAGE; j++) {// Traverse through all rows in the page
            // Check if the row is inactive
            if (!is_active(bm, j)) {
                page->rows[j].roll = roll; // Insert the new row into the page
                strcpy(page->rows[j].name, name);
                mark_active(bm, j); // Mark the row as active in the bitmap
                table->active_rows[i]++; // Increase the count of active rows in the page
                
                CacheNode* node = find_cache_node(i);
                if (node) node->changed = true;
                
                printf(":D Inserted Row: Page Number: %d, Row Number: %d, Roll: %d, Name: %s\n", i, j, roll, name);
                
                Data* new_data = (Data*)malloc(sizeof(Data));
                new_data->key = roll;
                new_data->page_no = i;
                new_data->slot = j;
                insert(table->btree_index, new_data);
                free(new_data);
                
                return true;
            }
        }
    }
    printf("Table is full, cannot insert new row.\n");
     // If no inactive row is found in any page, print a message indicating that the table is full and cannot insert the new row
    return false; // Return false to indicate that the insertion failed

}

void delete_row(table* table, int roll){
    
    Data* found = btree_search(table->btree_index, roll);
    if (found) {
        Page* page = get_page(table, found->page_no);
        if (page && is_active(page->bitmap, found->slot)) {
            Row* row = &page->rows[found->slot];
            printf("Deleting Row: Page Number: %d, Row Number: %d, Roll: %d, Name: %s\n",
                   found->page_no, found->slot, row->roll, row->name);
            
            table->active_rows[found->page_no]--;
            mark_inactive(page->bitmap, found->slot);
            row->roll = 0;
            memset(row->name, 0, sizeof(row->name));
            
            CacheNode* node = find_cache_node(found->page_no);
            if (node) node->changed = true;
            
            removeFromTree(table->btree_index, roll);
            free(found);
            return;
        }
        free(found);
    }
    printf("Roll number %d not found in the table. =(\n", roll);

}


Page* get_page(table* table, int page_no) {
    // Page* cached = get_page_from_cache(page_no); // if page is cached already, return it
    // if (cached) return cached;

    CacheNode* node = find_cache_node(page_no);
    if (node) return node->page;
    // Not in cache?, load from disk
    Page* page = create_page();
    fseek(table->data_pt, page_no * sizeof(Page), 0);
    fread(page, sizeof(Page), 1, table->data_pt);
    insert_page_in_cache(page_no, page, table->data_pt); // insert page in cache
    return page;
}

void table_close (table* table) {

    flush_cache(table->data_pt);
    fclose(table->data_pt);
    
    BTree_destroy(table->btree_index);
    
    for (int i = 0; i < TOTAL_PAGES; i++) {
        if (table->pages[i]) {
            free(table->pages[i]);
        }
    }
    free(table);
    return;

}