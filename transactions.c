#include "transactions.h"
#include "synchronization.h"
#include "user_queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void initAccount(Account *account, const char *accountNumber, double initialBalance) {
    strcpy(account->accountNumber, accountNumber);
    account->balance = initialBalance;
    printf("Creating account\n");
    createAccount(accountNumber, initialBalance);
    pthread_mutex_init(&account->lock, NULL);  // Initialize the mutex
    
}

void createAccount(const char* accountNumber, double initialBalance) {
    printf("Creating file for %s\n", accountNumber);
    char filename[256];
    sprintf(filename, "%s.txt", accountNumber);
    FILE* file = fopen(filename, "w"); // Create a new file or overwrite an existing one
    if (file) {
        fprintf(file, "%.2f", initialBalance);
        fclose(file);
        printf("Account file created successfully: %s\n", filename);
    } else {
        perror("Failed to create account file");
        printf("Failed to create file: %s\n", filename);
    }
}

double getAccountBalance(const char* accountNumber) {
    char filename[256];
    sprintf(filename, "%s.txt", accountNumber);
    printf("Attempting to open file: %s for %s\n", filename, accountNumber);  // Debug print
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open account file");
        return -1;
    }
    double balance;
    if (fscanf(file, "%lf", &balance) != 1) {
        fclose(file);
        return -1;
    }
    fclose(file);
    return balance;
}

void updateAccountBalance(const char* accountNumber, double newBalance) {
    char filename[256];
    sprintf(filename, "%s.txt", accountNumber);
    printf("Attempting to update file: %s\n", filename);  // Debug print
    FILE* file = fopen(filename, "w");
    if (file) {
        fprintf(file, "%.2f", newBalance);
        fclose(file);
    } else {
        perror("Failed to update account file");
    }
}



void closeAccount(const char* accountNumber) {
    printf("Closing the account.\n");
    char filename[256];
    sprintf(filename, "%s.txt", accountNumber);
    printf("%s\n", filename);
    if (remove(filename) != 0) {
        perror("Failed to close account");
    }
}

void processTransaction(Account *account, const Transaction* transaction) {
    printf("Entered account %s and processing type %d\n", transaction->accountNumber, transaction->transactionType); //second account cannot get past this
    enterAccount(account);

    switch (transaction->transactionType) {
        case CREATE:
            createAccount(transaction->accountNumber, transaction->amount);
            break;
        case DEPOSIT:
            // Simplified example; ensure to handle errors and edge cases in real scenarios
            double balance = getAccountBalance(transaction->accountNumber);
            if (balance >= 0) {
                updateAccountBalance(transaction->accountNumber, balance + transaction->amount);
            }
            break;
        case WITHDRAW:
            balance = getAccountBalance(transaction->accountNumber);
            if (balance >= transaction->amount) {
                updateAccountBalance(transaction->accountNumber, balance - transaction->amount);
            }
            break;
        case INQUIRY:
            balance = getAccountBalance(transaction->accountNumber);
            printf("Account %s balance: %.2f\n", transaction->accountNumber, balance);
            break;
        case TRANSFER:
            // Add logic for handling transfers:
            //(PSEUDO)
            //already in sender account so,
            //enter recipient account
            //withdraw from sender
            //deposit to recipient
            //exit recipient account
            break;
        case CLOSE:
            closeAccount(transaction->accountNumber);
            break;
        default:
            printf("Unknown transaction type.\n");
            break;
    }

    exitAccount(account);
}

const char* getTransactionTypeString(transType type) {
    switch (type) {
        case CREATE: return "CREATE";
        case DEPOSIT: return "DEPOSIT";
        case WITHDRAW: return "WITHDRAW";
        case INQUIRY: return "INQUIRY";
        case TRANSFER: return "TRANSFER";
        case CLOSE: return "CLOSE";
        default: return "UNKNOWN";
    }
}
