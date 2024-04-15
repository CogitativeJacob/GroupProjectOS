#ifndef TRANSACTIONS_H
#define TRANSACTIONS_H

#include <pthread.h>

typedef struct {
    char accountNumber[20];
    double balance;
    pthread_mutex_t lock;
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

void createAccount(const char* accountNumber, double initialBalance);
double getAccountBalance(const char* accountNumber);
void updateAccountBalance(const char* accountNumber, double newBalance);
void closeAccount(const char* accountNumber);

const char* getTransactionTypeString(transType type);  // Declaration

void initAccount(Account *account, const char *accountNumber, double initialBalance);
void processTransaction(Account *account, const Transaction *transaction);

#endif
