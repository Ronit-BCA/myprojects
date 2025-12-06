#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // Required for generating potentially 'random' PINs

// --- Constants and Structs ---

#define MAX_HISTORY 5 // Stores last 5 transactions
#define PIN_LENGTH 4
#define MAX_ATTEMPTS 3
#define MAX_USERS 3 // Supports up to 3 different users

typedef struct {
    char type[20]; // "Deposit", "Withdrawal", "Balance Inquiry"
    double amount; // The amount for deposit/withdrawal, 0 for inquiry
} Transaction;

typedef struct {
    char name[50];
    long long accountNumber;
    int pin;
    double balance;
    Transaction history[MAX_HISTORY];
    int historyCount;
} Account;

// --- Global Variables ---
Account users[MAX_USERS];
Account *currentUser = NULL; // Pointer to the currently logged-in user's account

// --- Function Prototypes ---
void initAccounts();
int findAccount(int inputPin);
void generatePIN(int *pin, int userId);
void displayMenu();
void balanceEnquiry();
void deposit();
void withdrawal();
void showHistory();
void addTransaction(const char *type, double amount);

// --- Main Function ---
int main() {
    int choice;
    int inputPin;
    int attempts = 0;
    int userIndex = -1;
    int username;

    

    // 1. Initialize Multiple Account Data (First-time setup and PIN generation)
    initAccounts();
    printf("\n+========WELCOME TO THE ATM SIMULATOR========+>\n");
    




    // 2. PIN Validation and Login
    while (attempts < MAX_ATTEMPTS) {
        printf("Enter your %d-digit PIN: ", PIN_LENGTH);
        if (scanf("%d", &inputPin) != 1) {
            printf("\nINVALID INPUT. PLEASE ENTER A NUMBER.\n",PIN_LENGTH);
            // Clear the input buffer
            while(getchar() != '\n'); 
            attempts++;
            continue;
        }
        
        // Input validation for PIN format
        if (inputPin < 1000 || inputPin > 9999) {
            printf("\nInvalid PIN format. PIN must be %d digits.\n", PIN_LENGTH);
            attempts++;
            continue;
        }

        userIndex = findAccount(inputPin);
        
        if (userIndex != -1) {
            currentUser = &users[userIndex]; // Set the current user pointer
            break; // Login successful
        } else {
            attempts++;
            printf("\nIncorrect PIN. %d attempts remaining.\n", MAX_ATTEMPTS - attempts);
        }
    }

    if (currentUser == NULL) {
        printf("\nACCOUNT ACCESS LOCKED. TOO MANY INCORRECT ATTEMPTS. EXITING.\n");
        return 0;
    }

    printf("\n+==== LOGIN SUCCESSFUL! WELCOME ====+, %s.\n", currentUser->name);

    // 3. Main Transaction Loop
    do {
        displayMenu();
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("\nINVALID INPUT. PLEASE ENTER A NUMBER.\n");
            // Clear the input buffer
            while(getchar() != '\n'); 
            choice = 0; 
            continue;
        }

        switch (choice) {
            case 1:
                balanceEnquiry();
                break;
            case 2:
                deposit();
                break;
            case 3:
                withdrawal();
                break;
            case 4:
                showHistory();
                break;
            case 5:
                printf("\nTHANK YOU FOR USING THE ATM SIMULATOR. GOODBYE!\n");
                break;
            default:

                printf("\nINVALID CHOICE. PLEASE SELECT AN OPTION FROM 1 TO 5.\n");
        }
        
        if (choice != 5) {
            printf("\nPRESS ENTER TO CONTINUE.......\n");
            while(getchar() != '\n');
            getchar();
        }

    } while (choice != 5);

    return 0;
}

// --- Function Definitions ---


void initAccounts() {
    // User 1
    printf("ENTER YOUR NAME : ");
    scanf("%s",&users[0].name);
    strcpy(users[0].name,users[0].name);
    srand(time(NULL));
    users[0].accountNumber = 11110000;
    users[0].balance = 5000.00;
    users[0].historyCount = 0;
    generatePIN(&users[0].pin, 1); 
    FILE *file = fopen("users.dat","wb");
    fwrite(&users,sizeof(users),1,file);
    fclose(file);
    
}


int findAccount(int inputPin) {
    for (int i = 0; i < MAX_USERS; i++) {
        if (users[i].pin == inputPin) {
            return i;
        }
    }
    return -1;
}


void generatePIN(int *pin, int userId) {
    
    *pin = (rand() % 9000) + 1000;
    printf("\nGenerated PIN for User %d %s: %d \n", userId, users[userId-1].name, *pin);
    
    
    
}


void displayMenu() {
    printf("\n<====== ATM Menu for %s ========>\n", currentUser->name);
    printf("1. PRESS 1 TO Balance Enquiry\n");
    printf("2. PRESS 2 TO Deposit\n");
    printf("3. PRESS 3 TO Withdrawal\n");
    printf("4. PRESS 4 TO Transaction History\n");
    printf("5. PRESS 5 TO Exit\n");
    printf("<===================================>\n");
}


void balanceEnquiry() {
    printf("\n=== BALANCE ENQUIRY ===\n");
    printf("Current Account Balance: $%.2f\n", currentUser->balance);
    addTransaction("Balance Inquiry", 0.0);
}

// DEPOSIT
void deposit() {
    double amount;
    printf("\n=== DEPOSIT ===\n");
    printf("Enter amount to deposit: $ ");
    
    if (scanf("%lf", &amount) != 1 || amount <= 0) {
        printf("\n*** ERROR: INVALID DEPOSIT AMOUNT. MUST BE A POSITIVE NUMBER. ***\n");
        while(getchar() != '\n');
        return;
    }

    currentUser->balance += amount;
    printf("\nSUCCESSFULLY DEPOSIT OF $%.2f.\n", amount);
    printf("NEW BALANCE: $%.2f\n", currentUser->balance);
    addTransaction("Deposit", amount);
}

// WITHDRAWAL
void withdrawal() {
    double amount;
    printf("\n=== WITHDRAWAL ===\n");
    printf("ENTER AMOUNT TO WITHDRAW: $ ");
    // Validation 1: Check for valid numeric input and positive amount
    if (scanf("%lf", &amount) != 1 || amount <= 0) {
        printf("\n*** ERROR: INVALID WITHDRAWAL AMOUNT. MUST BE A POSITIVE NUMBER. ***\n");
        while(getchar() != '\n');
        return;
    }

    // Validation 2: Check for sufficient balance
    if (amount > currentUser->balance) {
        printf("\n*** ERROR: Insufficient Balance. Current Balance: $%.2f ***\n", currentUser->balance);
        addTransaction("Withdrawal (Failed)", amount); // Record failed transaction
        return;
    }

    currentUser->balance -= amount;
    printf("\nSUCCESSFULLY WITHDRAWAL OF $%.2f.\n", amount);
    printf("NEW BALANCE: $ %.2f\n", currentUser->balance);
    addTransaction("Withdrawal", amount);
}

//TRANSACTION HISTORY
void showHistory() {
    printf("\n<=== TRANSACTION HISTORY (Last %d) ===>\n", MAX_HISTORY);

    if (currentUser->historyCount == 0) {
        printf("NO TRANSACTIONS RECORDED YET\n");
        return;
    }

    printf("%-25s %s\n", "Type", "Amount");
    printf("=====================================\n");
    
    // Logic for circular buffer display
    int display_start = (currentUser->historyCount > MAX_HISTORY) ? currentUser->historyCount - MAX_HISTORY : 0;

    for (int i = display_start; i < currentUser->historyCount; i++) {
        int index = i % MAX_HISTORY; 
        
        printf("%-25s ", currentUser->history[index].type);
        
        if (strcmp(currentUser->history[index].type, "Balance Inquiry") != 0) {
             printf("$%.2f\n", currentUser->history[index].amount);
        } else {
             printf("---\n");
        }
    }
}

//TRANSACTION HISTORY
void addTransaction(const char *type, double amount) {
    if (currentUser == NULL) return; 

    // Calculate the index for the circular buffer (0 to MAX_HISTORY - 1)
    int index = currentUser->historyCount % MAX_HISTORY; 
    
    strcpy(currentUser->history[index].type, type);
    currentUser->history[index].amount = amount;
    
    currentUser->historyCount++;
}