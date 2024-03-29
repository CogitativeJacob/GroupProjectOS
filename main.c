#include "process_management.h"
#include "shared_memory.h"
#include "synchronization.h"
#include "transactions.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

//global monitor queue
MonitorQueue mq = {NULL, NULL, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER};

int main() {
    // // Initial setup: read transactions, initialize shared memory, etc.
    
    // // Process transactions
    
    // int shmid = initSharedMemory(1024); // 1024 bytes for example
    // char* shmaddr = (char*) attachSharedMemory(shmid, 0);

    // // Write something to shared memory
    // strcpy(shmaddr, "Hello, shared memory!");

    // // Print what's in shared memory
    // printf("%s\n", shmaddr);

    // // Detach from shared memory
    // detachSharedMemory(shmaddr);

    // // Cleanup shared memory
    // cleanupSharedMemory(shmid);

    // // Cleanup and exit

    //Transaction testing (commented out above section)

    //initialize queue

    // Example transaction
    Transaction transaction = {DEPOSIT, "123456789", 100.0, "2023-03-26"};

    pid_t pid;
    for (int i = 0; i < 5; i++) { // Simulate 5 concurrent transactions
        pid = fork();
        //printf("Im alive: %d\n", pid);
        if (pid == 0) { // Child process
            enterMonitor(&mq, &transaction);
            exitMonitor(&mq);
            return 0;
        }
    }

    while (wait(NULL) > 0); // Wait for all child processes to finish

    return 0;
}
