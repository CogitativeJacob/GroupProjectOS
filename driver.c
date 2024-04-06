#include "synchronization.h"
#include "transactions.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


workerQueue cq = {NULL, NULL, CREATE, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER};
workerQueue dq = {NULL, NULL, DEPOSIT, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER};
workerQueue wq = {NULL, NULL, WITHDRAW, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER};
workerQueue iq = {NULL, NULL, INQUIRY, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER};
workerQueue tq = {NULL, NULL, TRANSFER, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER};
workerQueue clq = {NULL, NULL, CLOSE, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER};

int num = 0;
MonitorQueue mq = {NULL, NULL, &cq, &dq, &wq, &iq, &tq, &clq, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER};

int main() {
    pid_t workers[6];
    pid_t users[2];
    
    // Example transactions
    Transaction transactions1[] = {
        {DEPOSIT, "Account1.txt", 100.0, "2023-03-26", NULL},
        {WITHDRAW, "Account1.txt", 50.0, "2023-03-27", NULL},
        {DEPOSIT, "Account1.txt", 200.0, "2023-03-28", NULL},
        {INQUIRY, "Account1.txt", 100.0, "2023-03-29", NULL} // Assuming TRANSFER transactions include a recipient account
    };

    printf("Creating workers\n");
    // create worker threads
    for (int i = 0; i < 6; i++) {
        pid_t pid = fork();
        if (pid == 0) { // Child process
            // Process transactions for the account
            //while(num == 0){
                switch (i){
                    case CREATE:
                        printf("%d -- %d\n", i, getpid());
                        popjob(mq.createQueue, &num);
                        break;
                    case DEPOSIT:
                        printf("%d -- %d\n", i, getpid());
                        popjob(mq.depositQueue, &num);
                        break;
                    case WITHDRAW:
                        printf("%d -- %d\n", i, getpid());
                        popjob(mq.withdrawQueue, &num);
                        break;
                    case INQUIRY:
                        printf("%d -- %d\n", i, getpid());
                        popjob(mq.inquiryQueue, &num);
                        break;
                    case TRANSFER:
                        printf("%d -- %d\n", i, getpid());
                        popjob(mq.transferQueue, &num);
                        break;
                    case CLOSE:
                        printf("%d -- %d\n", i, getpid());
                        popjob(mq.closeQueue, &num);
                        break;
                    default:
                        printf("failed to find correct queue");
                        break;
                }
            //}
            printf("worker dies");
            exit(0);
        }
    }

    printf("Creating users\n");
    for (int i = 0; i < 2; i++) {
        pid_t pid = fork();
        if (pid == 0) { // Child process
            // Process transactions for the account
            for(int j = 0; j < 4; j++){
                //remove transaction from user queue and add to monitor queue
                enterMonitor(&mq, &transactions1[j]);
                printf(transactions1[j].timestamp);
                exitMonitor(&mq);
            }
            exit(0);
        }
    }

    

    // Parent process waits for all child processes to complete
    for (int i = 0; i < 2; i++) {
        wait(NULL);
    }

    num = 1;
    for (int i = 0; i < 6; i++) {
        wait(NULL);
    }

    return 0;
}
