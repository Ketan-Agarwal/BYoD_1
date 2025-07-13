#include "functions.h"
#include "tree.h"
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
    new_table->pages[i] = NULL; // No page is loaded initially
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

    // open/create index file
    new_table->index_pt = fopen("index.db", "rb+");
    if (!new_table->index_pt) {
        new_table->index_pt = fopen("index.db", "wb+");
        if (!new_table->index_pt) {
            perror("Failed to create/open index.db\n");
            fclose(new_table->data_pt);
            free(new_table);
            return NULL;
        }
    }

    return new_table; // Return the newly created table
}

bool search_table(table* table, int roll){
// function to search for a specific row in table
    // loop over all pages
    for (int i = 0; i < TOTAL_PAGES; i++){
        Page* page = get_page(table, i);
        
        // checks if page is empty or not
        if (!page) continue;

        // check gets bitmap for rows of that page
        uint8_t* bm = page->bitmap;

        // loop over all rows
        for (int j = 0; j < ROWS_PER_PAGE; j++){
            
            // check if row contains any data or not
            if (!is_active(bm, j)) continue;
            Row row = page->rows[j];
            
            if (row.roll == roll) {
                printf("Page Number: %d\nRow Number: %d\nRoll: %d\nName: %s", i, j, roll, row.name);
                return true; // If the roll number matches, print the row details and return true
            }
        }

    }
    return false; // If no matching row is found, return false
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

    for (int i = 0; i < TOTAL_PAGES; i++){// Traverse through all pages
        // Page* page = table->pages[i];
        Page* page = get_page(table, i);
        if(!page){// If the page is NULL, it means it has not been allocated yet
            page = create_page(); // Create a new page
            if (!page) return false; // If page creation fails, return false
            insert_page_in_cache(i, page, table->data_pt); // insert the newly created page in cache
            table->pages[i] = page; // Assign the newly created page to the table
        }
        else if (table->active_rows[i] == ROWS_PER_PAGE) continue; // If the page is full, skip to the next page

        uint8_t* bm = page->bitmap;// Get the bitmap array for the current page, where each bit represents the status of a row
        for (int j = 0; j < ROWS_PER_PAGE; j++){// Traverse through all rows in the page
            // Check if the row is inactive
            if (!is_active(bm, j)){
                page->rows[j].roll = roll; // Insert the new row into the page
                strcpy(page->rows[j].name, name);
                mark_active(bm, j); // Mark the row as active in the bitmap
                table->active_rows[i]++;  // Increase the count of active rows in the page

                CacheNode* node = find_cache_node(i);
                if (node) node->changed = true;

                printf(":D Inserted Row: Page Number: %d, Row Number: %d, Roll: %d, Name: %s\n", i, j, roll, name);
                index_insert(&root, roll, i, j); // Insert the row into the index tree
                return true; // Return true to indicate successful insertion
            }
        }
    }
    printf("Table is full, cannot insert new row.\n");
    // If no inactive row is found in any page, print a message indicating that the table is full and cannot insert the new row
    return false; // Return false to indicate that the insertion failed

}

void delete_row(table* table, int roll){
    
    for (int i = 0; i < TOTAL_PAGES; i++){// Traverse through all pages
        Page* page = get_page(table, i);
        // If the page is NULL or has no active rows, skip it
        if (!page) continue;
        // Get the bitmap array for the current page, where each bit represents the status of a row 
        uint8_t* bm = page->bitmap;

        for (int j = 0; j < ROWS_PER_PAGE; j++){// Traverse through all rows in the page
            // Check if the row is active
            if (!is_active(bm, j)) continue;
            Row* row = &(page->rows[j]);
            
            if (row->roll == roll) {// If the roll number matches, delete the row
                printf(" Deleting Row: Page Number: %d, Row Number: %d, Roll: %d, Name: %s\n", i, j, row->roll, row->name);
                table->active_rows[i]--;// Decrease the count of active rows in the page
                mark_inactive(bm, j);// Mark the row as inactive in the bitmap
                row->roll = 0; // Clear the roll number
                for(int k = 0; k < 28; k++) {
                    row->name[k] = '\0'; // Mask the name with null characters
                }

                CacheNode* node = find_cache_node(i);
                node->changed = true;

                // if(table->active_rows[i] == 0) {
                //     free(page); // If there are no active rows left in the page, free the page memory
                //     table->pages[i] = NULL; // Set the page pointer to NULL
                // }
                
                index_delete(&root, roll); // Delete the row from the index tree
                return;
            }
        }
    }
    printf("Roll number %d not found in the table. =( \n", roll);// If the roll number is not found, print a message
 
}


Page* get_page(table* t, int page_no) {
    // Page* cached = get_page_from_cache(page_no); // if page is cached already, return it
    // if (cached) return cached;

    CacheNode* node = find_cache_node(page_no);
    if (node) return node->page;
    // Not in cache?, load from disk
    Page* page = create_page();
    fseek(t->data_pt, page_no * sizeof(Page), 0);
    fread(page, sizeof(Page), 1, t->data_pt);
    insert_page_in_cache(page_no, page, t->data_pt); // insert page in cache
    return page;
}

void table_close (table* t) {
    flush_cache(t->data_pt);    // Write back changed data pages
    // flush_index_cache(t->index_pt); // Same for index pages, if needed

    fclose(t->data_pt);
    fclose(t->index_pt);
    for (int i = 0; i < TOTAL_PAGES; i++) {// Free all allocated pages in the table
    // Check if the page is not NULL before freeing it
    if (t->pages[i]) {
        free(t->pages[i]);
    }
}
    free(t);
}