#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "myutils.h"
#include "cache.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int valid_roll(char* input); // Validate the roll number input, ensuring it is a positive integer and within the range of INT_MAX
bool adjust_name(char* name, size_t max_len); // Adjusts the name to ensure it is null-terminated and does not exceed max length
// It also removes any newline character if present
bool search_table (table* table, int roll); // Search for a row by roll number returning true if found, false otherwise
void print_table (table* table); // Print the entire table
bool insert_row (table* table, int roll, const char* name); // Insert a new row into the table and returns true if successful
// If insertion fails, it returns false
void delete_row (table* table, int roll); // Delete a row from the table by roll number
Page* create_page(); // Create a new page
table* create_table(); // Create a new table
Page* get_page(table* table, int page_no); // get page pointer either from cache or from disk
void table_close(table* table);

#endif