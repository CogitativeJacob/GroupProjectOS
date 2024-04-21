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

    printf("Num Users: %d\n", numUsers);
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
        double amount = 0.0;
        int scanCount = sscanf(line, "%s %s %lf %s", accountNumber, transTypeStr, &amount, recipientAccountNumber);

        // Skip lines with incorrect format

        printf("Scancount: %d Transaction: %s, %s\n", scanCount, accountNumber, transTypeStr);

        Transaction transaction;
        memset(&transaction, 0, sizeof(Transaction)); // Clear the transaction structure
        strcpy(transaction.accountNumber, accountNumber);
        transaction.transactionType = parseTransactionType(transTypeStr);
        transaction.amount = amount;

        // Set recipientAccountNumber for transfer transactions
        if (transaction.transactionType == TRANSFER) {
            if (scanCount == 4) {
                strcpy(transaction.recipientAccountNumber, recipientAccountNumber);
            } else {
                fprintf(stderr, "Missing recipient account number for transfer transaction.\n");
                continue;
            }
        }

        Account* account = (transaction.transactionType == CREATE) ? 
            createAccount(accountNumber, transaction.amount) : 
            findAccount(accountNumber);

        if (!account) {
            switch (transaction.transactionType) {
                case DEPOSIT:
                    printf("Deposit failed because account %s does not exist.\n", transaction.accountNumber);
                    break;
                case WITHDRAW:
                    printf("Withdraw failed because account %s does not exist.\n", transaction.accountNumber);
                    break;
                case INQUIRY:
                    printf("Inquiry failed because account %s does not exist.\n", transaction.accountNumber);
                    break;
                case TRANSFER:
                    printf("Transfer failed because account %s does not exist.\n", transaction.accountNumber);
                    break;
                case CLOSE:
                    printf("Close failed because account %s does not exist.\n", transaction.accountNumber);
                    break;
            }
            continue;
        }

        // Link the transaction to the account and enqueue it
        transaction.account = account;
        enqueueTransaction(accountNumber, transaction);

        // Debugging output
        printf("Transaction parsed: %s, %s, %.2f, %s\n",
               transaction.accountNumber,
               getTransactionTypeString(transaction.transactionType),
               transaction.amount,
               transaction.recipientAccountNumber[0] ? transaction.recipientAccountNumber : "N/A");
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

    queue->isActive = 0;

    return NULL;
}


void processAllTransactionsConcurrently() {
    pthread_t threads[userCount];
    for (int i = 0; i < userCount; i++) {
        //printf("%s\n", userQueues[i].accountNumber); //Debug print
        pthread_create(&threads[i], NULL, threadProcessTransactions, &userQueues[i]);
        printf("Made thread for userCount %d\n", userCount);
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

    printUserQueues();
    //processAllTransactionsConcurrently(); // Process all transactions concurrently

    return 0;

}

