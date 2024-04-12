/*
* Group G
* Jacob Hathaway
* jacob.q.hathaway@okstate.edu
* 4/7/2024
*/

#ifndef UTILS_H
#define UTILS_H

#include "transactions.h"

#define MAX_USERS 100
#define MAX_LINE_LENGTH 256

extern char uniqueUsers[MAX_USERS][20];
extern int numUniqueUsers;


static void trackUser(const char* accountNumber);
static Transaction* createTransactionFromLine(const char* line);
void parseAndEnqueueTransactions(const char* filename);
void processUserTransactions(const char* accountNumber);

#endif