// transactions.h
#ifndef TRANSACTIONS_H
#define TRANSACTIONS_H

typedef struct {
    char transactionType[10];
    char accountNumber[20];
    double amount;
    char status[10];
    char timestamp[20];
    char recipientAccount[20];
} TransactionDetail;

#endif
