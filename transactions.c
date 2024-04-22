/*
* Group G
* Jacob Hathaway
* jacob.q.hathaway@okstate.edu
* 4/21/2024
*/

#include "transactions.h"
#include "synchronization.h"
#include "user_queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Account allAccounts[MAX_USERS]; // Global array to hold all account structures

/**
 * Creates a new account or returns an existing one if it already exists.
 * @param accountNumber Unique identifier for the account.
 * @param initialBalance Initial monetary value to set the account to upon creation.
 * @return Pointer to the Account structure, or NULL if creation failed due to reaching capacity.
 */
Account* createAccount(const char* accountNumber, double initialBalance) {
    // Check if the account already exists

    //printf("CreateAccount called for %s\n", accountNumber);

    for (int i = 0; i < userCount; i++) {
        if (strcmp(allAccounts[i].accountNumber, accountNumber) == 0) {
            // Account already exists, return a pointer to it
            return &allAccounts[i];
        }
    }

    // If we reached the maximum number of accounts, we cannot create more
    if (userCount >= MAX_USERS) {
        fprintf(stderr, "Maximum number of users reached. Cannot create account for %s\n", accountNumber);
        return NULL;
    }

    // Create the account file with the initial balance
    printf("Creating file for %s with initial balance: %.2f\n", accountNumber, initialBalance);
    char filename[256];
    sprintf(filename, "%s.txt", accountNumber);
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Failed to create account file");
        return NULL;
    }
    fprintf(file, "%.2f", initialBalance);
    fclose(file);
    //printf("Account file created successfully: %s\n", filename);

    // Initialize the new Account in the allAccounts array
    Account* newAccount = &allAccounts[userCount];
    strcpy(newAccount->accountNumber, accountNumber);
    newAccount->balance = initialBalance;
    pthread_mutex_init(&newAccount->lock, NULL);
    pthread_cond_init(&newAccount->cond, NULL);
    newAccount->closed = false;

    return newAccount;
}


/**
 * Retrieves the balance of a specified account.
 * @param accountNumber The account identifier.
 * @return The balance of the account, or -1 if the account file cannot be opened.
 */
double getAccountBalance(const char* accountNumber) {
    char filename[256];
    sprintf(filename, "%s.txt", accountNumber);
    //printf("Attempting to open file: %s for %s\n", filename, accountNumber);  // Debug print
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

/**
 * Updates the balance of a specified account.
 * @param accountNumber The account identifier.
 * @param newBalance The new balance to set.
 */
void updateAccountBalance(const char* accountNumber, double newBalance) {
    char filename[256];
    sprintf(filename, "%s.txt", accountNumber);
    //printf("Attempting to update file: %s\n", filename);  // Debug print
    FILE* file = fopen(filename, "w");
    if (file) {
        fprintf(file, "%.2f", newBalance);
        fclose(file);
    } else {
        perror("Failed to update account file");
    }
}

/**
 * Searches for an account by its number.
 * @param accountNumber The account identifier to search for.
 * @return Pointer to the Account if found, NULL otherwise.
 */
Account* findAccount(const char* accountNumber){
    //printf("FindAccount called, usercount: %d\n", userCount);
    for (int i = 0; i < userCount; i++) {
        if (strcmp(allAccounts[i].accountNumber, accountNumber) == 0) {
            return &allAccounts[i];
        }
    }
    return NULL; // Account number not found
}



/**
 * Closes an account by deleting its file.
 * @param accountNumber The account identifier.
 */
void closeAccount(const char* accountNumber) {
    //printf("Closing the account.\n");
    char filename[256];
    sprintf(filename, "%s.txt", accountNumber);
    //printf("%s\n", filename);
    if (remove(filename) != 0) {
        perror("Failed to close account");
    }
}

/**
 * Processes a transaction for a specified account.
 * @param accountNumber The account to process the transaction for.
 * @param transaction A pointer to the Transaction structure containing details about the transaction.
 */
void processTransaction(const char* accountNumber, const Transaction* transaction) {

    Account* account = findAccount(accountNumber);

    enterAccount(account);
    
    if(account->closed && transaction->transactionType != CREATE){
        printf("Cannot perform transaction because account %s is closed.\n", account->accountNumber);
        exitAccount(account);
        return;
    }
    //printf("Closed: %d, Transaction: %d\n", account->closed, transaction->transactionType); //Debug print
    

    switch (transaction->transactionType) {
        case CREATE:
            // account->closed = false;
            // createAccount(transaction->accountNumber, transaction->amount);
            break;
        case DEPOSIT:
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
            //check recipient exists
            //printf("Recipient account number for transfer: %s\n", transaction->recipientAccountNumber);
            Account* reciever = findAccount(transaction->recipientAccountNumber);
            //printf("Account: %s transferring to account: %s\n", transaction->accountNumber, reciever->accountNumber);
            if(reciever == NULL){
                printf("Cannot transfer $%.2f from %s to %s because recieving account does not exist.\n", transaction->amount, transaction->accountNumber, transaction->recipientAccountNumber);
                break;
            }

            //check if account was closed
            if(reciever->closed){
                printf("Cannot transfer $%.2f from %s to %s because recieving account was closed.\n", transaction->amount, transaction->accountNumber, transaction->recipientAccountNumber);
                break;
            }else{
                //prevent deadlock
                pthread_cond_wait(&reciever->cond, &account->lock);
            }

            //enter recipient account
            enterAccount(reciever);
            //check if account was closed
            if(reciever->closed){
                printf("Cannot transfer $%.2f from %s to %s because recieving account was closed.\n", transaction->amount, transaction->accountNumber, transaction->recipientAccountNumber);
                exitAccount(reciever);
                break;
            }
            //withdraw from sender
            balance = getAccountBalance(transaction->accountNumber);
            if (balance >= transaction->amount) {
                updateAccountBalance(transaction->accountNumber, balance - transaction->amount);
            }else{
                printf("Cannot transfer $%.2f from %s to %s due lack of funds.\n", balance, transaction->accountNumber, transaction->recipientAccountNumber);
                break;
            }
            //deposit to recipient
            balance = getAccountBalance(transaction->recipientAccountNumber);
            if (balance >= 0) {
                updateAccountBalance(transaction->recipientAccountNumber, balance + transaction->amount);
            }
            //exit recipient account
            exitAccount(reciever);
            break;
        case CLOSE:
            account->closed = true;
            closeAccount(transaction->accountNumber);
            break;
        default:
            printf("Unknown transaction type.\n");
            break;
    }

    exitAccount(account);
}

/**
 * Converts a transaction type enum to its string representation.
 * @param type The transaction type enum.
 * @return The string representation of the transaction type.
 */
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
