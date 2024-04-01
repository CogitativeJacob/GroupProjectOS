#include "user_queue.h"
#include "transactions.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void processUserTransactions(const char* accountNumber) {
    // Assuming transactions for this user have been enqueued
    // This function simulates processing those transactions in a separate process
    printf("Processing transactions for account: %s\n", accountNumber);
    Transaction* transaction;
    while ((transaction = dequeueTransaction(accountNumber)) != NULL) {
        printf("Processed transaction for %s: Type %d, Amount %.2f\n",
               accountNumber, transaction->transactionType, transaction->amount);
        appendTransactionToFile(transaction);
        // Here you would call enterMonitor, process the transaction, and then exitMonitor
        free(transaction); // Assume dynamically allocated
    }
}

int main() {
    
    // Example transactions
    Transaction transactions[] = {
        {DEPOSIT, "Account1", 100.0, "2023-03-26"},
        {WITHDRAW, "Account1", 50.0, "2023-03-27"},
        {DEPOSIT, "Account2", 200.0, "2023-03-26"},
        {TRANSFER, "Account2", 100.0, "2023-03-27", "Account1"} // Assuming TRANSFER transactions include a recipient account
    };
    int numTransactions = sizeof(transactions) / sizeof(transactions[0]);

    for (int i = 0; i < numTransactions; i++) {
        enqueueTransaction(transactions[i].accountNumber, &transactions[i]);
    }

    const char* uniqueAccounts[] = {"Account1", "Account2"};
    int numAccounts = sizeof(uniqueAccounts) / sizeof(uniqueAccounts[0]);

    for (int i = 0; i < numAccounts; i++) {
        pid_t pid = fork();
        if (pid == 0) { // Child process
            // Process transactions for the account
            processUserTransactions(uniqueAccounts[i]);
            exit(0);
        }
    }

    // Parent process waits for all child processes to complete
    for (int i = 0; i < numAccounts; i++) {
        wait(NULL);
    }



    return 0;
}
