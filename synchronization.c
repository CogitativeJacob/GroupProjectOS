#include "synchronization.h"
#include "transactions.h"
#include <stdio.h>
#include <pthread.h>

Monitor accountMonitor;

void initMonitor() {
    pthread_mutex_init(&accountMonitor.lock, NULL);
    pthread_cond_init(&accountMonitor.cond, NULL);
}

void enterAccount(Account *account) {
    pthread_mutex_lock(&account->lock);  // Lock the specific account
    //printf("Entered account: %s\n", account->accountNumber);
}

void exitAccount(Account *account) {
    pthread_mutex_unlock(&account->lock);  // Unlock the specific account
    pthread_cond_signal(&account->cond);
    //printf("Exited account: %s\n", account->accountNumber);
}
