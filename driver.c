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

void readAndEnqueueTransactions(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open transactions file");
        return;
    }

    char line[256];
    // Skip the first line that contains the number of users
    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file)) {
        char accountNumber[20], transTypeStr[20], recipientAccountNumber[20] = {0};
        double amount;
        int scanCount = sscanf(line, "%s %s %lf %s", accountNumber, transTypeStr, &amount, recipientAccountNumber);

        if (scanCount < 2) {
            fprintf(stderr, "Incorrect format in transactions file.\n");
            continue;
        }

        Transaction transaction;
        memset(&transaction, 0, sizeof(Transaction)); // Clear the transaction structure
        strcpy(transaction.accountNumber, accountNumber);
        transaction.transactionType = parseTransactionType(transTypeStr);
        transaction.amount = (scanCount >= 3) ? amount : 0.0; // Set amount if present

        Account* account = NULL;
        if (transaction.transactionType == CREATE) {
            account = createAccount(accountNumber, transaction.amount); // Create account for CREATE transactions
        } else {
            account = findAccount(accountNumber); // For other transactions, find existing account
        }

        if (!account) {
            fprintf(stderr, "No account found or created for %s\n", accountNumber);
            continue; // Skip this transaction if no account is found or created
        }

        // Link the transaction to the account
        transaction.account = account;

        // Enqueue the transaction
        enqueueTransaction(accountNumber, transaction);
    }

    fclose(file);
}




void* threadProcessTransactions(void* arg) {
    TransactionQueue* queue = (TransactionQueue*) arg;
    if (queue == NULL) {
        fprintf(stderr, "Error: Queue pointer is NULL.\n");
        return NULL;
    }

    //printf("Processing queue for account: %s\n", queue->accountNumber ? queue->accountNumber : "NULL");

    Transaction* transaction = dequeueTransaction(queue->accountNumber);
    while (transaction != NULL) {
        if (transaction->account == NULL) {
            fprintf(stderr, "Error: Account pointer is NULL for transaction.\n");
        } else {
            /*printf("Processing transaction for account: %s of type: %s\n",
                transaction->account->accountNumber ? transaction->account->accountNumber : "NULL",
                getTransactionTypeString(transaction->transactionType)); */

            processTransaction(transaction->accountNumber, transaction);
        }
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
    
    readAndEnqueueTransactions("input.txt");
    
    printUserQueues();
    
    processAllTransactionsConcurrently();
    //processAllTransactionsConcurrently(); // Process all transactions concurrently

    return 0;

}

