#ifndef SHARED_MEM_H
#define SHARED_MEM_H

#include "transactions.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct LogEntry {
    char message[40]; // Or some structure that represents a log message
    // Other relevant log entry details like timestamp, transaction ID, etc.
} LogEntry;

typedef struct TransactionLog {
    LogEntry* entries;
    int count;
    int capacity;
    pthread_mutex_t log_mutex; // Mutex for synchronizing access to the log
} TransactionLog;

extern TransactionLog transactionLog;

void initTransactionLog(int capacity);
void writeToLog(const char* message);
void destroyTransactionLog();
void saveLogToFile(const char* filename);
void printLogFile(const char* filename);

#endif