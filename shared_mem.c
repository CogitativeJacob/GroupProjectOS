#include "shared_mem.h"

void initTransactionLog(int capacity) {
    transactionLog.entries = malloc(capacity * sizeof(LogEntry));
    transactionLog.count = 0;
    transactionLog.capacity = capacity;
    pthread_mutex_init(&transactionLog.log_mutex, NULL);
}

void writeToLog(const char* message) {
    pthread_mutex_lock(&transactionLog.log_mutex);
    if (transactionLog.count < transactionLog.capacity) {
        snprintf(transactionLog.entries[transactionLog.count].message, sizeof(LogEntry), "%s", message);
        // Set other details for LogEntry as necessary
        transactionLog.count++;
    }
    pthread_mutex_unlock(&transactionLog.log_mutex);
}

void destroyTransactionLog() {
    free(transactionLog.entries);
    pthread_mutex_destroy(&transactionLog.log_mutex);
}

void saveLogToFile(const char* filename) {
    pthread_mutex_lock(&transactionLog.log_mutex);
    FILE* file = fopen(filename, "w");
    if (file != NULL) {
        for (int i = 0; i < transactionLog.count; i++) {
            fprintf(file, "%s\n", transactionLog.entries[i].message);
        }
        fclose(file);
    }
    pthread_mutex_unlock(&transactionLog.log_mutex);
}

void printLogFile(const char* filename){
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open transactions file");
        return;
    }

    char line[256];

    while (fgets(line, sizeof(line), file)) {
        printf("%s\n", line);
    }
}
