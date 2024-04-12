#include "transactions.h"
#include <stdio.h>
#include <stdlib.h>

// Simulate the shared memory or log file operation
void appendTransactionToFile(Transaction* transaction) {
    FILE* log = fopen("transaction_log.txt", "a");  // Log file for transaction history
    if (log != NULL) {
        fprintf(log, "%d, %s, %.2f, %s, %s\n",
                transaction->transactionType,
                transaction->accountNumber,
                transaction->amount,
                transaction->recipientAccountNumber,
                transaction->timestamp);
        fclose(log);
    } else {
        perror("Failed to open transaction log file");
    }

    // Create or modify the account file based on the transaction type
    char filename[25];
    sprintf(filename, "%s.txt", transaction->accountNumber);
    FILE* file;
    double currentBalance = 0.0;

    switch (transaction->transactionType) {
        case CREATE:
            file = fopen(filename, "w");
            if (file) {
                fprintf(file, "%.2f", transaction->amount);  // Initial balance
            }
            break;
        case DEPOSIT:
        case WITHDRAW:
        case INQUIRY:
        case TRANSFER:
        case CLOSE:
            file = fopen(filename, "r+");
            if (file) {
                fscanf(file, "%lf", &currentBalance);
                rewind(file);
                if (transaction->transactionType == DEPOSIT) {
                    currentBalance += transaction->amount;
                } else if (transaction->transactionType == WITHDRAW) {
                    currentBalance -= transaction->amount;  // Check for balance availability should be done beforehand
                }
                fprintf(file, "%.2f", currentBalance);
            }
            break;
        default:
            printf("Unsupported transaction type\n");
    }

    if (file) {
        fclose(file);
    }
}
