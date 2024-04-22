/*
* Group G
* Hagen Patterson
* hagen.patterson@okstate.edu
* 4/21/2024
*/

#ifndef SYNCHRONIZATION_H
#define SYNCHRONIZATION_H

#include "transactions.h"
#include <pthread.h>

typedef struct Monitor {
    pthread_mutex_t lock;       // Mutex for protecting the monitor
    pthread_cond_t cond;        // Condition variable for managing the queue
} Monitor;

extern Monitor accountMonitor; // Declare a monitor for account transactions

void enterAccount(Account *account); //Enters Account, uses pointer to account object to reference later
void exitAccount(Account *account); //Exits account, uses pointer to account object to reference later

void initMonitor();             // Function to initialize the monitor

#endif
