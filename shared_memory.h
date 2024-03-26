#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <sys/ipc.h>
#include <sys/shm.h>

// Function prototypes
int initSharedMemory(size_t size);
void* attachSharedMemory(int shmid, int shmflg);
void detachSharedMemory(const void *shmaddr);
void cleanupSharedMemory(int shmid);

#endif
