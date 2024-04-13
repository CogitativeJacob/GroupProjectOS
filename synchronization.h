#ifndef SYNCHRONIZATION_H
#define SYNCHRONIZATION_H

#include "transactions.h"
#include <pthread.h>

typedef struct Monitor {
    pthread_mutex_t lock;       // Mutex for protecting the monitor
    pthread_cond_t cond;        // Condition variable for managing the queue
} Monitor;

extern Monitor accountMonitor; // Declare a monitor for account transactions

void enterAccount(Account *account);
void exitAccount(Account *account);

void initMonitor();             // Function to initialize the monitor
void enterMonitor(Transaction* transaction);
void leaveMonitor();

#endif
