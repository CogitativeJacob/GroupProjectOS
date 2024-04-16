#include "transactions.h"
#include "synchronization.h"
#include "user_queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// void initAccount(Account *account, const char *accountNumber, double initialBalance) {
//     strcpy(account->accountNumber, accountNumber);
//     account->balance = initialBalance;
//     printf("Creating account\n");
//     createAccount(accountNumber, initialBalance);
//     pthread_mutex_init(&account->lock, NULL);  // Initialize the mutex
    
// }

Account* createAccount(const char* accountNumber, double initialBalance) {
    printf("Creating file for %s\n", accountNumber);
    char filename[256];
    sprintf(filename, "%s.txt", accountNumber);
    FILE* file = fopen(filename, "w"); // Create a new file or overwrite an existing one

    Account* account = (Account*) malloc(sizeof(Account));
    if (!account) {
        fprintf(stderr, "Memory allocation failed for account struct\n");
        return NULL;
    }

    if (file) {
        fprintf(file, "%.2f", initialBalance);
        fclose(file);
        printf("Account file created successfully: %s\n", filename);

        // Initialize the account struct
        strcpy(account->accountNumber, accountNumber);
        account->balance = initialBalance;
        pthread_mutex_init(&account->lock, NULL); // Initialize the mutex
        account->closed = false; // Initially, the account is open

        return account;
    } else {
        perror("Failed to create account file");
        printf("Failed to create file: %s\n", filename);
        free(account); // Avoid memory leak if file creation fails
        return NULL;
    }
}

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

Account* findAccount(const char* accountNumber){
    for (int i = 0; i < userCount; i++) {
        if (strcmp(userQueues[i].accountNumber, accountNumber) == 0) {
            return userQueues[i].front->transaction.account;
        }
    }
    return NULL;
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

void processTransaction(const char* accountNumber, const Transaction* transaction) {
    //printf("Entered account %s and processing type %d\n", transaction->accountNumber, transaction->transactionType); //Debug print

    Account* account = findAccount(accountNumber);
    if (account != NULL) {
        enterAccount(account);
    }
    
    if(account->closed && transaction->transactionType != CREATE){
        printf("Cannot perform transaction because account is closed.\n");
        exitAccount(account);
        return;
    }
    printf("Closed: %d, Transaction: %d\n", account->closed, transaction->transactionType); //Debug print
    

    switch (transaction->transactionType) {
        case CREATE:
            account->closed = false;
            createAccount(transaction->accountNumber, transaction->amount);
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
            Account* reciever = findAccount(transaction->recipientAccountNumber);
            if(reciever == NULL){
                printf("Cannot transfer $%.2f from %s to %s because recieving account does not exist.\n", transaction->amount, transaction->accountNumber, transaction->recipientAccountNumber);
                break;
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
