#include "temp_queue.h"
#include "transactions.h"
#include <stdio.h>

int enQueueJob(userQueue* q, Transaction* t){
    pthread_mutex_lock(&q->lock); // first lock and add self to queue

    if(q->front == NULL){
        q->front = t; // if empty add to front
        q->rear = q->front;
    }else{
        q->rear->next = t; // else add to rear
        q->rear = q->rear->next;
    }

    printf("Added job");

    pthread_mutex_unlock(&q->lock); // unlock queue for next
}


Transaction* dequeue(userQueue* q){
    pthread_mutex_lock(&q->lock); // lock queue to remove self from front

    //printf("removing self from queue: %d\n", getpid());

    Transaction* nextNode = q->front->next;

    if(nextNode == NULL){
        q->front == NULL; // if only one in queue empty queue
        q->rear = q->front;
    }else{
        q->front->next = NULL; // else pop and move to next
        q->front = nextNode;
    }

    

    pthread_mutex_unlock(&q->lock); // unlock queue
}