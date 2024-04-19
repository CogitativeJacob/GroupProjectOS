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

void getNumUsers(const char* filename){
    FILE* file = fopen(filename, "r");
    char line[20];
    if (!file) {
        perror("Failed to open transactions file");
        return;
    }

    fgets(line, sizeof(line), file);

    fscanf(file, "%d", &numUsers);  // Read the number of users

    fclose(file);
}

void readAndEnqueueTransactions(const char* filename, Account** list) {
    int count = 0;

    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open transactions file");
        return;
    }

    //fscanf(file, "%d", &numUsers);  // Read the number of users

    char line[256];
    int skipline = 0; //Fixes the attempted parse of the numUsers line
    while (fgets(line, sizeof(line), file)) {
        char accountNumber[20], transTypeStr[20], recipientAccountNumber[20];
        double amount;
        int scanCount = sscanf(line, "%s %s %lf %s", accountNumber, transTypeStr, &amount, recipientAccountNumber);

        Transaction transaction;
        memset(&transaction, 0, sizeof(Transaction)); // Clear the transaction structure
        strcpy(transaction.accountNumber, accountNumber);
        transaction.transactionType = parseTransactionType(transTypeStr);

        if (transaction.transactionType == TRANSFER && scanCount == 4) {
            strcpy(transaction.recipientAccountNumber, recipientAccountNumber);
            transaction.amount = amount;
        } else if (scanCount == 3) {
            transaction.amount = amount;
        }

        if(skipline == 0){
           skipline++;
           continue;
        }
            
        printf("%s\n", accountNumber);
        Account newaccount = enqueueTransaction(accountNumber, transaction);

        if(strcmp(newaccount.accountNumber, "xx") != 0){
            int here = -1;
            for(int i = 0; i <= count; i++){
                if(strcmp(newaccount.accountNumber, list[i]) == 0){
                    here = i;
                }
            }
            if(here != -1){
                *list[count] = newaccount;
                count++;
            }
            
        }
        
    }

    fclose(file);
}



void* threadProcessTransactions(void* arg) {
    TransactionQueue* queue = (TransactionQueue*) arg;
    Transaction* transaction = dequeueTransaction(queue->accountNumber);
    printf("Dequeued transaction for account: %s\n", transaction->accountNumber);
    while (transaction != NULL) {
        //enterMonitor(transaction);
        printf("Processing transaction for account: %s of type: %d\n", transaction->accountNumber, transaction->transactionType); //Debug print
        processTransaction(transaction->accountNumber, transaction);
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

    getNumUsers("input.txt");
    Account* listAccounts = (Account*)malloc( numUsers * sizeof(Account) );

    initMonitor(); // Initialize synchronization mechanisms if any
    
    readAndEnqueueTransactions("input.txt", &listAccounts);

    printf("Accounts:\n");
    for(int i = 0; i < numUsers; i++){
        printf("%s\n", listAccounts[i].accountNumber);
    }
    
    printUserQueues();
    
    processAllTransactionsConcurrently();
    //processAllTransactionsConcurrently(); // Process all transactions concurrently

    return 0;

}

