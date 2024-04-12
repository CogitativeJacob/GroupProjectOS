/*
* Group G
* Abhiram Reddy Alugula
* aalugul@okstate.edu
* 4/7/2024
*/

#include "transactions.h"
#include <stdio.h>
#include <pthread.h>
#include <time.h>

// Global mutex for file access synchronization
//pthread_mutex_t fileMutex = PTHREAD_MUTEX_INITIALIZER;

// void appendTransactionToFile(Transaction* transaction) {
//     //pthread_mutex_lock(&fileMutex); // Lock the mutex before accessing the file
//     //Need to use shared memory here

//     FILE* file = fopen("log.txt", "a"); // Open the file in append mode
//     if (file != NULL) {
//         fprintf(file, "Type: %d, Account: %s, Amount: %.2f, Timestamp: %s\n",
//                 transaction->transactionType, transaction->accountNumber,
//                 transaction->amount, transaction->timestamp);
//         fclose(file); // Close the file after writing
//     } else {
//         perror("Failed to open transaction log file");
//     }

//     //pthread_mutex_unlock(&fileMutex); // Unlock the mutex after file operation is done
// }

#include "transactions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// Prototype for the transaction processing function
void* process_transaction(void* arg);

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE* file = fopen(argv[1], "r");
    if (!file) {
        perror("Failed to open input file");
        return 1;
    }

    int numTransactions;
    fscanf(file, "%d\n", &numTransactions);
    pthread_t threads[numTransactions];
    Transaction transactions[numTransactions];

    for (int i = 0; i < numTransactions; i++) {
        Transaction transaction;
        char typeStr[10];
        transaction.amount = 0;
        strcpy(transaction.recipientAccountNumber, "");

        fscanf(file, "%s %s", transaction.accountNumber, typeStr);
        if (strcmp(typeStr, "Create") == 0) transaction.transactionType = CREATE;
        else if (strcmp(typeStr, "Deposit") == 0) transaction.transactionType = DEPOSIT;
        else if (strcmp(typeStr, "Withdraw") == 0) transaction.transactionType = WITHDRAW;
        else if (strcmp(typeStr, "Inquiry") == 0) transaction.transactionType = INQUIRY;
        else if (strcmp(typeStr, "Transfer") == 0) {
            transaction.transactionType = TRANSFER;
            fscanf(file, "%lf %s", &transaction.amount, transaction.recipientAccountNumber);
        } else if (strcmp(typeStr, "Close") == 0) transaction.transactionType = CLOSE;
        else continue; // Skip unknown transaction types

        // For transactions with an amount (except Transfer, handled above)
        if (transaction.transactionType != TRANSFER && transaction.transactionType != INQUIRY && transaction.transactionType != CLOSE) {
            fscanf(file, "%lf", &transaction.amount);
        }

        transactions[i] = transaction;
        pthread_create(&threads[i], NULL, process_transaction, (void*)&transactions[i]);
    }

    // Joining threads to ensure all transactions are processed before program termination
    for (int i = 0; i < numTransactions; i++) {
        pthread_join(threads[i], NULL);
    }

    fclose(file);
    return 0;
}

double readBalance(const char* accountNumber) {
    pthread_mutex_lock(&fileMutex);
    double balance = -1;
    char filename[25];
    sprintf(filename, "%s.txt", accountNumber);

    FILE* file = fopen(filename, "r");
    if (file != NULL) {
        fscanf(file, "%lf", &balance);
        fclose(file);
    }
    pthread_mutex_unlock(&fileMutex);
    return balance;
}

void writeBalance(const char* accountNumber, double balance) {
    pthread_mutex_lock(&fileMutex);
    char filename[25];
    sprintf(filename, "%s.txt", accountNumber);

    FILE* file = fopen(filename, "w");
    if (file != NULL) {
        fprintf(file, "%.2f", balance);
        fclose(file);
    }
    pthread_mutex_unlock(&fileMutex);
}

void* process_transaction(void* arg) {
    Transaction* transaction = (Transaction*) arg;
    // Extract needed information from the transaction for convenience
    const char* accountNumber = transaction->accountNumber;
    const double amount = transaction->amount;
    const char* recipientAccountNumber = transaction->recipientAccountNumber;

    switch (transaction->transactionType) {
        case CREATE: {
            // To create an account, simply create a new file with the account number as its name
            // and write the initial balance (amount) into it.
            writeBalance(accountNumber, amount);
            break;
        }
        case DEPOSIT: {
            // For deposit, read the current balance from the file, add the deposit amount, and write back.
            double currentBalance = readBalance(accountNumber);
            if (currentBalance >= 0) {
                writeBalance(accountNumber, currentBalance + amount);
            } else {
                printf("Account %s does not exist.\n", accountNumber);
            }
            break;
        }
        case WITHDRAW: {
            // For withdraw, ensure the account has enough balance before deducting the amount.
            double currentBalance = readBalance(accountNumber);
            if (currentBalance >= amount) {
                writeBalance(accountNumber, currentBalance - amount);
            } else {
                printf("Insufficient funds in account %s.\n", accountNumber);
            }
            break;
        }
        case INQUIRY: {
            // Inquiry involves reading the balance and printing it out.
            double currentBalance = readBalance(accountNumber);
            if (currentBalance >= 0) {
                printf("Account %s balance: %.2f\n", accountNumber, currentBalance);
            } else {
                printf("Account %s does not exist.\n", accountNumber);
            }
            break;
        }
        case TRANSFER: {
            // Transfer requires checking the balance of the source account,
            // then deducting the amount from it and adding it to the recipient's account.
            double senderBalance = readBalance(accountNumber);
            if (senderBalance >= amount) {
                double recipientBalance = readBalance(recipientAccountNumber);
                if (recipientBalance >= 0) {
                    writeBalance(accountNumber, senderBalance - amount);
                    writeBalance(recipientAccountNumber, recipientBalance + amount);
                } else {
                    printf("Recipient account %s does not exist.\n", recipientAccountNumber);
                }
            } else {
                printf("Insufficient funds in account %s for transfer.\n", accountNumber);
            }
            break;
        }
        case CLOSE: {
            // To close an account, simply delete its file.
            char filename[30];
            sprintf(filename, "%s.txt", accountNumber);
            if (remove(filename) == 0) {
                printf("Account %s closed successfully.\n", accountNumber);
            } else {
                printf("Failed to close account %s.\n", accountNumber);
            }
            break;
        }
        default:
            printf("Unknown transaction type.\n");
            break;
    }
    return NULL;
}