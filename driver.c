/*
* Group G
* 4/21/2024
*/

#include "synchronization.h"
#include "transactions.h"
#include "user_queue.h"
#include "shared_mem.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

TransactionLog transactionLog;

/**
 * Parses a transaction type from a given string.
 * @param typeStr A string representing the transaction type.
 * @return The corresponding transType enum value for the given string.
 */
transType parseTransactionType(const char* typeStr) {
    if (strcmp(typeStr, "Create") == 0) return CREATE;
    if (strcmp(typeStr, "Deposit") == 0) return DEPOSIT;
    if (strcmp(typeStr, "Withdraw") == 0) return WITHDRAW;
    if (strcmp(typeStr, "Inquiry") == 0) return INQUIRY;
    if (strcmp(typeStr, "Transfer") == 0) return TRANSFER;
    if (strcmp(typeStr, "Close") == 0) return CLOSE;
    //return UNKNOWN;
}

/**
 * Reads transactions from a file and enqueues them for processing.
 * @param filename The name of the file containing transaction data.
 */
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

        //Sets transaction object up with the basic information.
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

        //creates account if transtype is create, otherwise finds account.
        Account* account = (transaction.transactionType == CREATE) ? 
            createAccount(accountNumber, transaction.amount) : 
            findAccount(accountNumber);

        if (!account) { //Handles failures due to account nonexistence. Also writes to temp buffer to write to log.
            char temp[80];
            switch (transaction.transactionType) {
                case DEPOSIT:
                    printf("Deposit failed because account %s does not exist.\n", transaction.accountNumber);
                    sprintf(temp, "%s %s %.2f failed", getTransactionTypeString(transaction.transactionType), transaction.accountNumber, transaction.amount);
                    break;
                case WITHDRAW:
                    printf("Withdraw failed because account %s does not exist.\n", transaction.accountNumber);
                    sprintf(temp, "%s %s %.2f failed", getTransactionTypeString(transaction.transactionType), transaction.accountNumber, transaction.amount);
                    break;
                case INQUIRY:
                    printf("Inquiry failed because account %s does not exist.\n", transaction.accountNumber);
                    sprintf(temp, "%s %s failed", getTransactionTypeString(transaction.transactionType), transaction.accountNumber);
                    break;
                case TRANSFER:
                    printf("Transfer failed because account %s does not exist.\n", transaction.accountNumber);
                    sprintf(temp, "%s %.2f %s %s failed", getTransactionTypeString(transaction.transactionType), transaction.amount, transaction.accountNumber, transaction.recipientAccountNumber);
                    break;
                case CLOSE:
                    printf("Close failed because account %s does not exist.\n", transaction.accountNumber);
                    sprintf(temp, "%s %s failed", getTransactionTypeString(transaction.transactionType), transaction.accountNumber);
                    break;
            }
            writeToLog(temp);
            continue;
        }

        // Link the transaction to the account and enqueue it
        transaction.account = account;
        enqueueTransaction(accountNumber, transaction);

        // Debugging output
        //printf("Transaction parsed: %s, %s, %.2f, %s\n", transaction.accountNumber, getTransactionTypeString(transaction.transactionType), transaction.amount, transaction.recipientAccountNumber[0] ? transaction.recipientAccountNumber : "N/A");
    }

    fclose(file);
}

/**
 * Thread function to process transactions for a specific account.
 * @param arg A pointer to the TransactionQueue for the account.
 * @return NULL upon completion of transaction processing.
 */
void* threadProcessTransactions(void* arg) {
    TransactionQueue* queue = (TransactionQueue*) arg;
    if (queue == NULL) {
        fprintf(stderr, "Error: Queue pointer is NULL.\n");
        return NULL;
    }

    //printf("Processing queue for account: %s\n", queue->accountNumber ? queue->accountNumber : "NULL");

    Transaction* transaction = dequeueTransaction(queue->accountNumber); //Grabs transaction from queue.
    while (transaction != NULL) {
        if (transaction->account == NULL) {
            fprintf(stderr, "Error: Account pointer is NULL for transaction.\n");
        } else {

            processTransaction(transaction->accountNumber, transaction);

            char temp[80];
            switch (transaction->transactionType) { //Writes successes to temp to write to log
                case CREATE:
                    sprintf(temp, "%s %s %.2f success", getTransactionTypeString(transaction->transactionType), transaction->accountNumber, transaction->amount);
                    break;
                case DEPOSIT:
                    sprintf(temp, "%s %s %.2f success", getTransactionTypeString(transaction->transactionType), transaction->accountNumber, transaction->amount);
                    break;
                case WITHDRAW:
                    sprintf(temp, "%s %s %.2f success", getTransactionTypeString(transaction->transactionType), transaction->accountNumber, transaction->amount);
                    break;
                case INQUIRY:
                    sprintf(temp, "%s %s success", getTransactionTypeString(transaction->transactionType), transaction->accountNumber);
                    break;
                case TRANSFER:
                    sprintf(temp, "%s %.2f %s %s success", getTransactionTypeString(transaction->transactionType), transaction->amount, transaction->accountNumber, transaction->recipientAccountNumber);
                    break;
                case CLOSE:
                    sprintf(temp, "%s %s success", getTransactionTypeString(transaction->transactionType), transaction->accountNumber);
                    break;
            }
            
            writeToLog(temp);
        }
        free(transaction);
        transaction = dequeueTransaction(queue->accountNumber);
    }

    queue->isActive = 0;

    return NULL;
}

/**
 * Spawns threads to process transactions for each user account concurrently.
 */
void processAllTransactionsConcurrently() {
    pthread_t threads[userCount];
    for (int i = 0; i < userCount; i++) {
        //printf("%s\n", userQueues[i].accountNumber); //Debug print
        //struct tester test = {logList, &userQueues[i]};
        
        pthread_create(&threads[i], NULL, threadProcessTransactions, &userQueues[i]);
        //printf("Made thread for userCount %d\n", userCount);
    }
    for (int i = 0; i < userCount; i++) {
        pthread_join(threads[i], NULL);
    }
}


//Main function that calls all functions needed.
int main() {

    initTransactionLog(40);
    
    readAndEnqueueTransactions("input.txt");
    
    printUserQueues();
    
    processAllTransactionsConcurrently();

    saveLogToFile("log.txt");

    destroyTransactionLog();

    printLogFile("log.txt");

    return 0;

}

