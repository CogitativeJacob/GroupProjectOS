#ifndef TRANSACTIONS_H
#define TRANSACTIONS_H

typedef enum{
    CREATE, DEPOSIT, WITHDRAW,
    INQUIRY, TRANSFER, CLOSE
} transType;

typedef struct {
    transType transactionType;
    char accountNumber[20];
    double amount;
    char timestamp[20]; // Simplified for example purposes
    struct Transaction* next; // Pointer to the next transaction in the queue
} Transaction;

void appendTransactionToFile(Transaction* transaction);

#endif
