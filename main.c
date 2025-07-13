#include <stdio.h>
#include "myutils.h"
#include "functions.h"


void clear_buffer()
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
}

int main(){
    table* my_table= create_table(); // Create a new table
    if (!my_table) {
        return 1; // If table creation fails, exit the program
    }
    int choice;
    while(1){
        printf("\nMenu:\n");
        printf("1. Insert Row\n");
        printf("2. Search Row\n");
        printf("3. Print Table\n");
        printf("4. Delete Row\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        int roll;
        char name[28]; 
        char input[20]; // Buffer to read the roll number input
        roll=0;
        switch (choice) {
        case 1:
            // Insert a new row into the table
            // Prompt user for roll number and name
            printf("Inserting a new row...\n");
            printf("Enter Roll Number: ");
            scanf("%s", input); // Read the input as a string
            roll = valid_roll(input); // Validate the roll number input
            if(roll==-1){
                continue; // If the roll number is invalid, skip this iteration
            }
            if(search_table(my_table, roll)) {
                printf("\nRoll number %d already exists. Please enter a unique roll number.\n", roll);
                continue; // If the roll number already exists, skip this iteration
            }
            printf("Enter Name: ");
            getchar();  // Consume newline character left by previous scanf
            fgets(name, sizeof(name), stdin); // Use fgets to read the name with spaces
            if(!adjust_name(name, MAX_NAME_LENGTH)) { // Adjust the name to ensure it is null-terminated and does not exceed max length
                printf("Invalid name. Please enter a name with less than %d characters.\n", MAX_NAME_LENGTH);
                continue; // Skip this iteration if the name is invalid
            }
            insert_row(my_table, roll, name); // Call the function to insert the new row
            break;
        case 2:
            // Search for a row in the table
            printf("Enter Roll Number to Search: ");
            scanf("%s", input); // Read the input as a string
            roll = valid_roll(input); // Validate the roll number input
            if (roll == -1) {
                continue; // If the roll number is invalid, skip this iteration
            }
            if (!search_table(my_table, roll)) {
                printf("Roll number %d not found.\n", roll);
            }
            break;

        case 3:
            // Print the entire table
            print_table(my_table);
            break;

        case 4:
            // Delete a row from the table
            printf("Enter Roll Number to Delete: ");
            scanf("%s", input); // Read the input as a string
            roll = valid_roll(input); // Validate the roll number input
            if (roll == -1) {
                continue; // If the roll number is invalid, skip this iteration
            }
            delete_row(my_table, roll);
            break;

        case 5:
            // Exit the program
            printf("Exiting...\n");
            table_close(my_table);
            return 0;

        default: printf("Invalid choice. Please try again.\n");
            break;
        }
        input[0] = '\0'; // Clear the input buffer
        name[0] = '\0'; // Clear the name buffer
        clear_buffer(); // Clear any remaining input in the buffer
    }
}