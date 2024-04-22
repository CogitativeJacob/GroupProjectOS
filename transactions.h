/*
* Group G
* Jacob Hathaway
* jacob.q.hathaway@okstate.edu
* 4/21/2024
*/

#ifndef TRANSACTIONS_H
#define TRANSACTIONS_H

#include <pthread.h>
#include <stdbool.h>

/**
 * Represents an individual bank account.
 */
typedef struct {
    char accountNumber[20];             // Unique identifier for the account
    double balance;                     // Current balance of the account
    pthread_mutex_t lock;               // Mutex for synchronizing access to this account
    pthread_cond_t cond;                // Condition variable for coordinating access to this account
    bool closed;                        // Indicates whether the account is open or closed
} Account;

/**
 * Enumeration of possible transaction types.
 */
typedef enum {
    CREATE,     // Transaction to create a new account
    DEPOSIT,    // Transaction to deposit money into an account
    WITHDRAW,   // Transaction to withdraw money from an account
    INQUIRY,    // Transaction to inquire about the current balance
    TRANSFER,   // Transaction to transfer money between accounts
    CLOSE       // Transaction to close an account
} transType;

/**
 * Represents a transaction in the banking system.
 */
typedef struct {
    transType transactionType;           // Type of transaction
    char accountNumber[20];              // Account number associated with the transaction
    Account* account;                    // Pointer to the Account structure this transaction affects
    double amount;                       // Amount involved in the transaction
    char recipientAccountNumber[20];     // Account number of the recipient (used in TRANSFER transactions)
    char timestamp[20];                  // Timestamp of when the transaction was logged (formatted string)
} Transaction;

// Function declarations for account management and transaction processing:

/**
 * Creates a new account with a specified initial balance.
 * @param accountNumber The identifier for the new account.
 * @param initialBalance The starting balance for the new account.
 * @return Pointer to the newly created Account structure.
 */
Account* createAccount(const char* accountNumber, double initialBalance);

/**
 * Retrieves the current balance of a specified account.
 * @param accountNumber The identifier of the account.
 * @return The current balance of the account.
 */
double getAccountBalance(const char* accountNumber);

/**
 * Updates the balance of a specified account.
 * @param accountNumber The identifier of the account.
 * @param newBalance The new balance to set for the account.
 */
void updateAccountBalance(const char* accountNumber, double newBalance);

/**
 * Closes a specified account.
 * @param accountNumber The identifier of the account to close.
 */
void closeAccount(const char* accountNumber);

/**
 * Converts a transaction type enumeration to its string representation.
 * @param type The transaction type enumeration.
 * @return The string representation of the transaction type.
 */
const char* getTransactionTypeString(transType type);

/**
 * Initializes an account structure with the provided details.
 * @param account Pointer to the Account structure to initialize.
 * @param accountNumber The identifier for the account.
 * @param initialBalance The starting balance for the account.
 */
void initAccount(Account *account, const char *accountNumber, double initialBalance);

/**
 * Processes a transaction for a specified account.
 * @param accountNumber The identifier of the account.
 * @param transaction Pointer to the Transaction structure containing transaction details.
 */
void processTransaction(const char* accountNumber, const Transaction *transaction);

/**
 * Finds an account in the system by its account number.
 * @param accountNumber The identifier of the account to find.
 * @return Pointer to the found Account structure, or NULL if not found.
 */
Account* findAccount(const char* accountNumber);

#endif
