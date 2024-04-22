/*
* Group G
* Hagen Patterson
* hagen.patterson@okstate.edu
* 4/21/2024
*/

#include "synchronization.h"
#include "transactions.h"
#include <stdio.h>
#include <pthread.h>

// Global structure to manage synchronization primitives used for the account operations
Monitor accountMonitor;

/**
 * Initializes the synchronization primitives used for handling account operations.
 * This function sets up the mutex and condition variable needed to manage exclusive
 * access to account data in a multi-threaded environment.
 */
void initMonitor() {
    // Initialize the mutex part of the accountMonitor.
    pthread_mutex_init(&accountMonitor.lock, NULL);
    // Initialize the condition variable part of the accountMonitor.
    pthread_cond_init(&accountMonitor.cond, NULL);
}

/**
 * Locks the mutex for a specific account to ensure exclusive access.
 * This function is called when a thread needs to perform operations on an account
 * and needs to ensure that no other thread modifies the account data simultaneously.
 * 
 * @param account A pointer to the Account structure for which the mutex should be locked.
 */
void enterAccount(Account *account) {
    // Lock the mutex associated with the account to prevent other threads from entering.
    pthread_mutex_lock(&account->lock);
    // Print a debug message indicating which account is currently locked by the thread.
    printf("Entered account: %s\n", account->accountNumber);
}

/**
 * Unlocks the mutex for a specific account and signals one waiting thread, if any,
 * that it can proceed with account operations.
 * This function is called when a thread has completed its operations on an account
 * and allows other threads to access the account data.
 * 
 * @param account A pointer to the Account structure for which the mutex should be unlocked.
 */
void exitAccount(Account *account) {
    // Unlock the mutex associated with the account to allow other threads to enter.
    pthread_mutex_unlock(&account->lock);
    // Signal the condition variable to wake up one thread waiting on this account's condition variable.
    pthread_cond_signal(&account->cond);
    // Print a debug message indicating which account is now unlocked.
    printf("Exited account: %s\n", account->accountNumber);
}
