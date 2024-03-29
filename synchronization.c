#include "synchronization.h"
#include "transactions.h"
#include <stdio.h>


void enterMonitor(MonitorQueue* q, Transaction* t) {
    //printf("entered 1: %d\n", getpid());
    pthread_mutex_lock(&q->lock); // first lock and add self to queue

    //printf("entered 2: %d\n", getpid());
    QueueNode newNode = {getpid(), NULL};

    if(q->front == NULL){
        q->front = &newNode; // if empty add to front
        q->rear = q->front;
        //printf("first: %d\n", getpid());
    }else{
        //printf("entered 3: %d\n", getpid());
        q->rear->next = &newNode; // else add to rear
        q->rear = q->rear->next;
    }
    
    

    //printf("Added self to q: %d\n", getpid());

    while(q->front->pid != getpid()){
        //printf("still waiting on signal: %d -- %d\n", getpid(), q->front->pid);
        pthread_cond_wait(&q->cond, &q->lock); // wait of not in front of queue
    }
    
    appendTransactionToFile(t); // do work

    //printf("appended: %d\n", getpid());

    pthread_mutex_unlock(&q->lock);
}

void exitMonitor(MonitorQueue* q){
    pthread_mutex_lock(&q->lock);

    //printf("removing self from queue: %d\n", getpid());

    QueueNode* nextNode = q->front->next;

    if(nextNode == NULL){
        q->front == NULL; // if only one in queue empty queue
    }else{
        q->front->next = NULL; // else pop and move to next
        q->front = nextNode;
    }

    

    pthread_mutex_unlock(&q->lock);

    //printf("signaling: %d\n", getpid());
    pthread_cond_signal(&q->cond);
}