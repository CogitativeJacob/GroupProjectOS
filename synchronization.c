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
}

void exitAccount(Account *account) {
    pthread_mutex_unlock(&account->lock);  // Unlock the specific account
}
