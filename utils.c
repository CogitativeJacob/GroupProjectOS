#include "utils.h"
#include "user_queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USERS 100
#define MAX_LINE_LENGTH 256

char uniqueUsers[MAX_USERS][20]; // Array to store unique account numbers
int numUniqueUsers = 0;

// Checks if a user is already tracked and adds them if not
static void trackUser(const char* accountNumber) {
    for (int i = 0; i < numUniqueUsers; i++) {
        if (strcmp(uniqueUsers[i], accountNumber) == 0) {
            return; // User already tracked
        }
    }
    if (numUniqueUsers < MAX_USERS) {
        strcpy(uniqueUsers[numUniqueUsers++], accountNumber);
    } else {
        printf("Max users limit reached.\n");
    }
}

// Parses a line to create a Transaction object (dynamically allocated)
static Transaction* createTransactionFromLine(const char* line) {
    char accountNumber[20], transactionTypeStr[20];
    double amount = 0.0;
    char recipientAccount[20] = ""; // For transfers
    transType type;

    int scannedItems = sscanf(line, "%s %s %lf %s", accountNumber, transactionTypeStr, &amount, recipientAccount);
    
    // Map transactionTypeStr to transType enum
    if (strcmp(transactionTypeStr, "Withdraw") == 0) type = WITHDRAW;
    else if (strcmp(transactionTypeStr, "Deposit") == 0) type = DEPOSIT;
    else if (strcmp(transactionTypeStr, "Transfer") == 0) type = TRANSFER;
    else if (strcmp(transactionTypeStr, "Create") == 0) type = CREATE;
    else if (strcmp(transactionTypeStr, "Inquiry") == 0) type = INQUIRY;
    else if (strcmp(transactionTypeStr, "Close") == 0) type = CLOSE;
    else {
        printf("Unknown transaction type: %s\n", transactionTypeStr);
        return NULL;
    }

    Transaction* transaction = (Transaction*)malloc(sizeof(Transaction));
    if (!transaction) {
        perror("Failed to allocate memory for transaction");
        exit(EXIT_FAILURE);
    }

    strcpy(transaction->accountNumber, accountNumber);
    transaction->transactionType = type;
    transaction->amount = (type == WITHDRAW || type == DEPOSIT || type == TRANSFER) ? amount : 0;
    strcpy(transaction->timestamp, ""); // Timestamp not used in this example
    transaction->next = NULL;

    // For TRANSFER transactions, set the recipient account
    if (type == TRANSFER && scannedItems == 4) {
        strcpy(transaction->recipientAccountNumber, recipientAccount);
    }

    return transaction;
}

void parseAndEnqueueTransactions(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Could not open transactions file.");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];
    fgets(line, sizeof(line), file); // Skip the first line (number of transactions)
    while (fgets(line, sizeof(line), file)) {
        Transaction* transaction = createTransactionFromLine(line);
        if (transaction) {
            enqueueTransaction(transaction->accountNumber, transaction);
            trackUser(transaction->accountNumber);
        }
    }

    fclose(file);
}

// Define or declare other necessary functions like isUserTracked, enqueueTransaction here or in their respective files.
