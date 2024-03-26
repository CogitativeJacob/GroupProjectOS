#ifndef TRANSACTIONS_H
#define TRANSACTIONS_H

typedef struct {
    char transactionType[10];
    char accountNumber[20];
    double amount;
    char timestamp[20]; // Simplified for example purposes
} Transaction;

void appendTransactionToFile(Transaction* transaction);

#endif
