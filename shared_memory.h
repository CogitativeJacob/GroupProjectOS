#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include "transactions.h" // Assuming TransactionDetail is defined here

int initSharedMemory(int numTransactions);
void cleanupSharedMemory(int shmid);

#endif
