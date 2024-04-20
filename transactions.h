#ifndef TRANSACTIONS_H
#define TRANSACTIONS_H

#include <pthread.h>
#include <stdbool.h>

typedef struct {
    char accountNumber[20];
    double balance;
    pthread_mutex_t lock;
    bool closed;     //check if account is still open
} Account;

typedef enum {
    CREATE, DEPOSIT, WITHDRAW, INQUIRY, TRANSFER, CLOSE
} transType;

typedef struct {
    transType transactionType;
    char accountNumber[20];
    Account* account; //keeps accounts
    double amount;
    char recipientAccountNumber[20]; // Only used for TRANSFER
    char timestamp[20];
} Transaction;

// In transactions.h
Account* createAccount(const char* accountNumber, double initialBalance);
double getAccountBalance(const char* accountNumber);
void updateAccountBalance(const char* accountNumber, double newBalance);
void closeAccount(const char* accountNumber);

const char* getTransactionTypeString(transType type);  // Declaration

void initAccount(Account *account, const char *accountNumber, double initialBalance);
void processTransaction(const char* accountNumber, const Transaction *transaction);
Account* findAccount(const char* accountNumber);

#endif
