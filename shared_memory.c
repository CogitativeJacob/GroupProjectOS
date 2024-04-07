/*
* Group G
* Jacob Hathaway
* jacob.q.hathaway@okstate.edu
* 4/7/2024
*/

#include "shared_memory.h"
#include <stdio.h>
#include <stdlib.h>

// Initialize shared memory and return the shared memory ID (shmid)
int initSharedMemory(size_t size) {
    key_t key = ftok("shmfile", 65); // Generate a unique key
    int shmid = shmget(key, size, 0666|IPC_CREAT);
    if (shmid == -1) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }
    return shmid;
}

// Attach to the shared memory segment and return a pointer to it
void* attachSharedMemory(int shmid, int shmflg) {
    void *shmaddr = shmat(shmid, NULL, shmflg);
    if (shmaddr == (void *) -1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }
    return shmaddr;
}

// Detach from the shared memory segment
void detachSharedMemory(const void *shmaddr) {
    if (shmdt(shmaddr) == -1) {
        perror("shmdt failed");
        exit(EXIT_FAILURE);
    }
}

// Remove the shared memory segment
void cleanupSharedMemory(int shmid) {
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl failed");
        exit(EXIT_FAILURE);
    }
}
