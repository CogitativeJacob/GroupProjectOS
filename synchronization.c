#include "synchronization.h"
#include "transactions.h"
#include <stdio.h>


void enterMonitor(MonitorQueue* q, Transaction* t) {
    pthread_mutex_lock(&q->lock); // first lock and add self to queue

    QueueNode newNode = {getpid(), NULL};

    if(q->front == NULL){
        q->front = &newNode; // if empty add to front
        q->rear = q->front;
    }else{
        q->rear->next = &newNode; // else add to rear
        q->rear = q->rear->next;
    }
    
    
    //printf("Added self to q: %d\n", getpid());

    while(q->front->pid != getpid()){
        //printf("still waiting on signal: %d -- %d\n", getpid(), q->front->pid);
        pthread_cond_wait(&q->cond, &q->lock); // wait of not in front of queue and unlock queue
    }

    printf("Adding job\n");
    switch (t->transactionType){
        case CREATE:
            //if(q->createQueue == NULL){
            //    workerQueue temp = {NULL, NULL, CREATE, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER};
            //    q->createQueue = &temp;
            //}
            addjob(q->createQueue, t);
            break;
        case DEPOSIT:
            addjob(q->depositQueue, t);
            break;
        case WITHDRAW:
            addjob(q->withdrawQueue, t);
            break;
        case INQUIRY:
            addjob(q->inquiryQueue, t);
            break;
        case TRANSFER:
            addjob(q->transferQueue, t);
            break;
        case CLOSE:
            addjob(q->closeQueue, t);
            break;
        default:
            break;
    }


    pthread_mutex_unlock(&q->lock); // unlock queue for next
}

void exitMonitor(MonitorQueue* q){
    pthread_mutex_lock(&q->lock); // lock queue to remove self from front

    printf("removing self from queue: %d\n", getpid());

    QueueNode* nextNode = q->front->next;

    if(nextNode == NULL){
        q->front == NULL; // if only one in queue empty queue
        q->rear = q->front;
    }else{
        q->front->next = NULL; // else pop and move to next
        q->front = nextNode;
    }

    

    pthread_mutex_unlock(&q->lock); // unlock queue

    //printf("signaling: %d\n", getpid());
    pthread_cond_signal(&q->cond); // signal that queue has been updated
}

void addjob(workerQueue* q, Transaction* t){
    printf("in addjob\n");
    pthread_mutex_lock(&q->jlock);
    printf("got addjob\n");

    jobNode newNode = {t, NULL};

    if(q->front == NULL){
        q->front = &newNode; // if empty add to front
        q->rear = q->front;
    }else{
        q->rear->next = &newNode; // else add to rear
        q->rear = q->rear->next;
    }

    printf("added job\n");

    pthread_mutex_unlock(&q->jlock); // unlock queue

    printf("signaling: %d\n", getpid());
    pthread_cond_signal(&q->jcond); // signal that queue has been updated
}

void popjob(workerQueue* q, int* noMoreJobs){
    pthread_mutex_lock(&q->jlock);

    while(q->front == NULL && !(*noMoreJobs)){
        printf("still waiting on work: %d\n", getpid());
        pthread_cond_wait(&q->jcond, &q->jlock); // wait if no jobs
    }

    printf("starting work");

    appendTransactionToFile(q->front->job);

    printf("work done\n");

    jobNode* nextNode = q->front->next;

    if(nextNode == NULL){
        q->front == NULL; // if only one in queue empty queue
        q->rear = q->front;
    }else{
        q->front->next = NULL; // else pop and move to next
        q->front = nextNode;
    }

    printf("removed job\n");

    pthread_mutex_unlock(&q->jlock);
}