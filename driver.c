#include "synchronization.h"
#include "transactions.h"
#include "user_queue.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int numUsers;

transType parseTransactionType(const char* typeStr) {
    if (strcmp(typeStr, "Create") == 0) return CREATE;
    if (strcmp(typeStr, "Deposit") == 0) return DEPOSIT;
    if (strcmp(typeStr, "Withdraw") == 0) return WITHDRAW;
    if (strcmp(typeStr, "Inquiry") == 0) return INQUIRY;
    if (strcmp(typeStr, "Transfer") == 0) return TRANSFER;
    if (strcmp(typeStr, "Close") == 0) return CLOSE;
    //return UNKNOWN;
}

void processTransactionDirectly(const char* accountNumber, const char* typeStr, double amount, const char* recipientAccountNumber) {
    Transaction transaction;
    strcpy(transaction.accountNumber, accountNumber);
    transaction.amount = amount;
    strcpy(transaction.recipientAccountNumber, recipientAccountNumber);
    transaction.transactionType = parseTransactionType(typeStr);

    enqueueTransaction(accountNumber, transaction);
}

void readAndEnqueueTransactions(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open transactions file");
        return;
    }

    fscanf(file, "%d", &numUsers);  // Number of transactions (assuming this is not actually the number of unique accounts)

    char accountNumber[20], transTypeStr[20], recipientAccountNumber[20];
    double amount;
    char line[256];

    while (fgets(line, sizeof(line), file)) {
        // Reset recipient account number each time
        memset(recipientAccountNumber, 0, sizeof(recipientAccountNumber));

        // Parse the transaction details from the line
        if (sscanf(line, "%s %s %lf %s", accountNumber, transTypeStr, &amount, recipientAccountNumber) == 4) {
            // It's a transfer with a recipient
            processTransactionDirectly(accountNumber, transTypeStr, amount, recipientAccountNumber);
        } else if (sscanf(line, "%s %s %lf", accountNumber, transTypeStr, &amount) == 3) {
            // Regular transaction without recipient
            processTransactionDirectly(accountNumber, transTypeStr, amount, "");
        } else {
            fprintf(stderr, "Invalid transaction format: %s", line);
        }
    }

    fclose(file);
}


void* threadProcessTransactions(void* arg) {
    TransactionQueue* queue = (TransactionQueue*) arg;
    Transaction* transaction = dequeueTransaction(queue->accountNumber);
    while (transaction != NULL) {
        //enterMonitor(transaction);
        //printf("Processing transaction for account: %s of type: %d\n", transaction->accountNumber, transaction->transactionType); //Debug print
        processTransaction(transaction->account, transaction);
        free(transaction);
        transaction = dequeueTransaction(queue->accountNumber);
    }
    return NULL;
}

void processAllTransactionsConcurrently() {
    pthread_t threads[userCount];
    for (int i = 0; i < userCount; i++) {
        //printf("%s\n", userQueues[i].accountNumber); //Debug print
        pthread_create(&threads[i], NULL, threadProcessTransactions, &userQueues[i]);
    }
    for (int i = 0; i < userCount; i++) {
        pthread_join(threads[i], NULL);
    }
}

//was used for testing
void listQueue(){
    TransactionQueue* queue = &userQueues[0];
    Transaction* transaction = dequeueTransaction(queue->accountNumber);
    while (transaction != NULL) {
        transaction = dequeueTransaction(queue->accountNumber);
    }
}



int main() {

    initMonitor(); // Initialize synchronization mechanisms if any
    readAndEnqueueTransactions("input.txt");
    printUserQueues();
    //processAllTransactionsConcurrently(); // Process all transactions concurrently


    return 0;

}

