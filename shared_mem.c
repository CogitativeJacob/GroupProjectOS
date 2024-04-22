/*
* Group G
* Hagen Patterson
* hagen.patterson@okstate.edu
* 4/21/2024
*/

#include "shared_mem.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

/**
 * Initializes the transaction log with a specified capacity.
 * Allocates memory for log entries and sets up necessary synchronization primitives.
 * 
 * @param capacity The maximum number of log entries the log can hold.
 */
void initTransactionLog(int capacity) {
    // Allocate memory for the number of entries specified by capacity
    transactionLog.entries = malloc(capacity * sizeof(LogEntry));
    transactionLog.count = 0;  // Initialize the current count of log entries to 0
    transactionLog.capacity = capacity;  // Set the capacity of the log
    // Initialize the mutex for synchronizing access to the log
    pthread_mutex_init(&transactionLog.log_mutex, NULL);
}

/**
 * Writes a message to the transaction log.
 * This function is thread-safe and ensures that access to the log is synchronized.
 * 
 * @param message The message to be logged.
 */
void writeToLog(const char* message) {
    // Lock the mutex before modifying the log to ensure exclusive access
    pthread_mutex_lock(&transactionLog.log_mutex);
    
    if (transactionLog.count < transactionLog.capacity) {
        // Get the current time and store it with the log entry
        transactionLog.entries[transactionLog.count].timestamp = time(NULL);
        
        // Store the message in the log entry
        snprintf(transactionLog.entries[transactionLog.count].message,
                 sizeof(LogEntry), "%s", message);
        // Increment the log count after adding the new entry
        transactionLog.count++;
    }
    
    // Unlock the mutex after updating the log
    pthread_mutex_unlock(&transactionLog.log_mutex);
}

/**
 * Cleans up the transaction log by freeing allocated memory and destroying the mutex.
 */
void destroyTransactionLog() {
    // Free the memory allocated for log entries
    free(transactionLog.entries);
    // Destroy the mutex used for log synchronization
    pthread_mutex_destroy(&transactionLog.log_mutex);
}

/**
 * Saves the contents of the transaction log to a file.
 * Each log entry is written with a timestamp.
 * 
 * @param filename The name of the file where the log should be saved.
 */
void saveLogToFile(const char* filename) {
    // Lock the mutex before reading the log to ensure no modifications occur while saving to file
    pthread_mutex_lock(&transactionLog.log_mutex);
    FILE* file = fopen(filename, "w");
    
    if (file != NULL) {
        for (int i = 0; i < transactionLog.count; i++) {
            char timeBuffer[30];
            struct tm *tm_info;

            // Convert the timestamp of each log entry to a human-readable format
            tm_info = localtime(&transactionLog.entries[i].timestamp);
            strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", tm_info);
            
            // Write the formatted log entry to the file
            fprintf(file, "%s [%s]\n", transactionLog.entries[i].message, timeBuffer);
        }
        fclose(file);
    }
    
    // Unlock the mutex after saving the log
    pthread_mutex_unlock(&transactionLog.log_mutex);
}

/**
 * Prints the contents of a log file to the console.
 * 
 * @param filename The name of the file containing the log to be printed.
 */
void printLogFile(const char* filename){
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open transactions file");
        return;
    }

    char line[256];

    // Read each line from the file and print it
    while (fgets(line, sizeof(line), file)) {
        printf("%s\n", line);
    }

    fclose(file);
}
