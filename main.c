#include "process_management.h"
#include "shared_memory.h"
#include "synchronization.h"
#include "transactions.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

int main() {
    // Initial setup: read transactions, initialize shared memory, etc.
    
    // Process transactions
    
    int shmid = initSharedMemory(1024); // 1024 bytes for example
    char* shmaddr = (char*) attachSharedMemory(shmid, 0);

    // Write something to shared memory
    strcpy(shmaddr, "Hello, shared memory!");

    // Print what's in shared memory
    printf("%s\n", shmaddr);

    // Detach from shared memory
    detachSharedMemory(shmaddr);

    // Cleanup shared memory
    cleanupSharedMemory(shmid);

    // Cleanup and exit
    return 0;
}
