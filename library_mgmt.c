#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_BOOKS 100
#define MAX_INPUT_SIZE 50

// Define Book Structure
struct Book {
    int id;
    char title[100];
    char author[50];
    int copies;
    int borrowed;
    char dueDate[15];
};

// Function to Authenticate User
int authenticate(char role[]) {
    char *username = malloc(MAX_INPUT_SIZE);
    char *password = malloc(MAX_INPUT_SIZE);
    if (!username || !password) {
        printf("Memory allocation failed!\n");
        free(username);
        free(password);
        return 0;
    }
    
    printf("Enter Username: ");
    scanf("%49s", username);
    printf("Enter Password: ");
    scanf("%49s", password);

    int authenticated = (strcmp(username, "admin") == 0 && strcmp(password, "admin123") == 0 && strcmp(role, "Admin") == 0) ||
                         (strcmp(username, "member") == 0 && strcmp(password, "member123") == 0 && strcmp(role, "Member") == 0);
    
    free(username);
    free(password);
    return authenticated;
}

// Function to Get Current Date and Set Due Date (7 days later)
void setDueDate(char dueDate[]) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    tm.tm_mday += 7;
    mktime(&tm);
    sprintf(dueDate, "%02d-%02d-%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
}

// Function to Add a Book (Admin Only)
void addBook() {
    FILE *file = fopen("library.dat", "ab");
    if (!file) return;

    struct Book b;
    printf("Enter Book ID: ");
    scanf("%d", &b.id);
    printf("Enter Book Title: ");
    scanf(" %99[^\n]", b.title);
    printf("Enter Author: ");
    scanf(" %49[^\n]", b.author);
    printf("Enter Number of Copies: ");
    scanf("%d", &b.copies);
    b.borrowed = 0;
    strcpy(b.dueDate, "Not Borrowed");

    fwrite(&b, sizeof(struct Book), 1, file);
    fclose(file);
}

// Function to Borrow a Book (Member Only)
void borrowBook() {
    FILE *file = fopen("library.dat", "rb+");
    if (!file) return;

    struct Book b;
    int id, found = 0;
    printf("Enter Book ID to Borrow: ");
    scanf("%d", &id);

    while (fread(&b, sizeof(struct Book), 1, file)) {
        if (b.id == id && b.copies > b.borrowed) {
            found = 1;
            b.borrowed++;
            setDueDate(b.dueDate);
            fseek(file, -(long)sizeof(struct Book), SEEK_CUR);
            fwrite(&b, sizeof(struct Book), 1, file);
            printf("Book borrowed successfully! Due Date: %s\n", b.dueDate);
            break;
        }
    }
    if (!found) printf("Book not available for borrowing.\n");
    fclose(file);
}

// Function to Return a Book (Member Only)
void returnBook() {
    FILE *file = fopen("library.dat", "rb+");
    if (!file) return;

    struct Book b;
    int id, found = 0;
    printf("Enter Book ID to Return: ");
    scanf("%d", &id);

    while (fread(&b, sizeof(struct Book), 1, file)) {
        if (b.id == id && b.borrowed > 0) {
            found = 1;
            b.borrowed--;
            strcpy(b.dueDate, "Not Borrowed");
            fseek(file, -(long)sizeof(struct Book), SEEK_CUR);
            fwrite(&b, sizeof(struct Book), 1, file);
            printf("Book returned successfully!\n");
            break;
        }
    }
    if (!found) printf("No borrowed copy of this book found.\n");
    fclose(file);
}

// Function to Display All Books (Both Admin and Member can view)
void displayBooks() {
    FILE *file = fopen("library.dat", "rb");
    if (!file) return;

    struct Book b;
    while (fread(&b, sizeof(struct Book), 1, file)) {
        printf("ID: %d | Title: %s | Author: %s | Copies: %d | Borrowed: %d | Due Date: %s\n",
               b.id, b.title, b.author, b.copies, b.borrowed, b.dueDate);
    }
    fclose(file);
}

// Main Menu
int main() {
    int choice;
    char role[10];
    
    printf("Enter role (Admin/Member): ");
    scanf("%9s", role);
    if (!authenticate(role)) {
        printf("Authentication failed. Exiting program.\n");
        return 0;
    }

    while (1) {
        printf("1. Add Book (Admin Only)\n");
        printf("2. Display Books\n");
        printf("3. Borrow Book (Member Only)\n");
        printf("4. Return Book (Member Only)\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        if (strcmp(role, "Admin") == 0 && choice == 1) {
            addBook();
        } else if (choice == 2) {
            displayBooks();
        } else if (choice == 3 && strcmp(role, "Member") == 0) {
            borrowBook();
        } else if (choice == 4 && strcmp(role, "Member") == 0) {
            returnBook();
        } else if (choice == 5) {
            printf("Exiting program. Goodbye!\n");
            return 0;
        } else {
            printf("Invalid choice or unauthorized action!\n");
        }
    }
}
