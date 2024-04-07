#include "synchronization.h"
#include "transactions.h"
#include <stdio.h>


void enterMonitor(MonitorQueue* q, Transaction* t) {
    pthread_mutex_lock(&q->MQlock); // first lock and add self to queue

    QueueNode newNode = {getpid(), NULL};

    if(q->front == NULL){
        q->front = &newNode; // if empty add to front
        q->rear = q->front;
    }else{
        q->rear->next = &newNode; // else add to rear
        q->rear = q->rear->next;
    }
    
    
    printf("Added self to q: %d\n", getpid());

    while(q->front->pid != getpid()){
        //printf("still waiting on signal: %d -- %d\n", getpid(), q->front->pid);
        pthread_cond_wait(&q->cond, &q->MQlock); // wait of not in front of queue and unlock queue
    }

    switch (t->transactionType)
    {
    case CREATE:
        pthread_mutex_lock(&q->lock0);
        if(pthread_create(&q->thread0, NULL, &grabWorker, (void *)t) != 0){
            printf("create worker not found");
        }else{
            pthread_join(q->thread0, NULL);
        }
        pthread_mutex_unlock(&q->lock0);
        break;
    case DEPOSIT:
        pthread_mutex_lock(&q->lock1);
        if(pthread_create(&q->thread1, NULL, &grabWorker, (void *)t) != 0){
            printf("deposit worker not found");
        }else{
            pthread_join(q->thread1, NULL);
        }
        pthread_mutex_unlock(&q->lock1);
        break;
    case WITHDRAW:
        pthread_mutex_lock(&q->lock2);
        if(pthread_create(&q->thread2, NULL, &grabWorker, (void *)t) != 0){
            printf("withdraw worker not found");
        }else{
            pthread_join(q->thread2, NULL);
        }
        pthread_mutex_unlock(&q->lock2);
        break;
    case INQUIRY:
        pthread_mutex_lock(&q->lock3);
        if(pthread_create(&q->thread3, NULL, &grabWorker, (void *)t) != 0){
            printf("inquiry worker not found");
        }else{
            pthread_join(q->thread3, NULL);
        }
        pthread_mutex_unlock(&q->lock3);
        break;
    case TRANSFER:
        pthread_mutex_lock(&q->lock4);
        if(pthread_create(&q->thread4, NULL, &grabWorker, (void *)t) != 0){
            printf("transfer worker not found");
        }else{
            pthread_join(q->thread4, NULL);
        }
        pthread_mutex_unlock(&q->lock4);
        break;
    case CLOSE:
        pthread_mutex_lock(&q->lock5);
        if(pthread_create(&q->thread5, NULL, &grabWorker, (void *)t) != 0){
            printf("close worker not found");
        }else{
            pthread_join(q->thread5, NULL);
        }
        pthread_mutex_unlock(&q->lock5);
        break;
    default:
        printf("Invalid transaction type");
        break;
    }

    pthread_mutex_unlock(&q->MQlock); // unlock queue for next
}

void exitMonitor(MonitorQueue* q){
    pthread_mutex_lock(&q->MQlock); // lock queue to remove self from front

    //printf("removing self from queue: %d\n", getpid());

    QueueNode* nextNode = q->front->next;

    if(nextNode == NULL){
        q->front == NULL; // if only one in queue empty queue
        q->rear = q->front;
    }else{
        q->front->next = NULL; // else pop and move to next
        q->front = nextNode;
    }

    

    pthread_mutex_unlock(&q->MQlock); // unlock queue

    //printf("signaling: %d\n", getpid());
    pthread_cond_signal(&q->cond); // signal that queue has been updated
}

void* grabWorker(void* transaction){

    // will separate transaction funtionality once threads are working
    appendTransactionToFile(transaction);

    return transaction;
}