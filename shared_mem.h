/*
* Group G
* Hagen Patterson
* hagen.patterson@okstate.edu
* 4/21/2024
*/

#ifndef SHARED_MEM_H
#define SHARED_MEM_H

#include "transactions.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/**
 * Represents a single entry in the transaction log.
 */
typedef struct LogEntry {
    char message[40]; // Stores a log message, typically a description of the transaction
    time_t timestamp; // Timestamp when the log entry was created
} LogEntry;

/**
 * Manages the entire set of log entries.
 * This structure acts as a container for all the log entries created during the
 * application's runtime, providing mechanisms for thread-safe access.
 */
typedef struct TransactionLog {
    LogEntry* entries;           // Pointer to an array of log entries
    int count;                   // Current number of entries in the log
    int capacity;                // Maximum number of entries the log can hold
    pthread_mutex_t log_mutex;   // Mutex for synchronizing access to the log
} TransactionLog;

// Global variable that holds the transaction log
extern TransactionLog transactionLog;

/**
 * Initializes the transaction log with a specified maximum capacity.
 * Allocates memory for the log entries and initializes synchronization primitives.
 * 
 * @param capacity Maximum number of entries the log can hold.
 */
void initTransactionLog(int capacity);

/**
 * Writes a log entry to the transaction log.
 * This function is thread-safe and ensures that access to the log is synchronized.
 * 
 * @param message The message to log.
 */
void writeToLog(const char* message);

/**
 * Frees resources associated with the transaction log.
 * This includes deallocating the log entries array and destroying the mutex.
 */
void destroyTransactionLog();

/**
 * Saves all current log entries to a specified file.
 * Each entry is written with its timestamp to the file.
 * 
 * @param filename The path to the file where the log will be saved.
 */
void saveLogToFile(const char* filename);

/**
 * Prints the contents of a log file.
 * This function opens the specified file, reads it line by line, and prints each line.
 * 
 * @param filename The path to the file to be printed.
 */
void printLogFile(const char* filename);

#endif // SHARED_MEM_H
