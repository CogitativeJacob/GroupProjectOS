/*
* Group G
* Abhiram Reddy Alugula
* aalugul@okstate.edu
* 4/7/2024
*/

#include "transactions.h"

// Helper function prototypes (if they are not included in transactions.h)
void create_account(const char* accountNumber, double initialDeposit);
void deposit(const char* accountNumber, double amount);
void withdraw(const char* accountNumber, double amount);
void transfer(const char* fromAccount, const char* toAccount, double amount);
double inquiry(const char* accountNumber);
void close_account(const char* accountNumber);
void process_transaction(SharedTransaction* transactions, int index);
void log_transaction(const SharedTransaction* txn);

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE* file = fopen(argv[1], "r");
    if (!file) {
        perror("Failed to open input file");
        return 1;
    }

    int numTransactions;
    fscanf(file, "%d\n", &numTransactions);

    int shm_id = shmget(SHM_KEY, sizeof(SharedTransaction) * MAX_TRANSACTIONS, IPC_CREAT | 0666);
    if (shm_id < 0) {
        perror("shmget failed");
        return 1;
    }

    SharedTransaction* transactions = (SharedTransaction*)shmat(shm_id, NULL, 0);
    if (transactions == (void*)-1) {
        perror("shmat failed");
        return 1;
    }

    memset(transactions, 0, sizeof(SharedTransaction) * MAX_TRANSACTIONS);

    for (int i = 0; i < numTransactions; i++) {
        fscanf(file, "%d %s %lf %s",
               (int*)&transactions[i].type,
               transactions[i].accountNumber,
               &transactions[i].amount,
               transactions[i].recipientAccountNumber);

        pid_t pid = fork();
        if (pid == 0) { // Child process
            process_transaction(transactions, i);
            exit(0);
        } else if (pid < 0) {
            perror("fork failed");
            fclose(file);
            return 1;
        }
    }

    while (wait(NULL) > 0); // Wait for all child processes to finish

    for (int i = 0; i < numTransactions; ++i) {
        log_transaction(&transactions[i]);
    }

    shmdt(transactions);
    shmctl(shm_id, IPC_RMID, NULL);
    fclose(file);
    return 0;
}

void create_account(const char* accountNumber, double initialDeposit) {
    char filename[255];
    snprintf(filename, sizeof(filename), "%s.txt", accountNumber);
    FILE* file = fopen(filename, "w");
    if (file) {
        fprintf(file, "%.2f\n", initialDeposit);
        fclose(file);
    }
}

void deposit(const char* accountNumber, double amount) {
    char filename[255];
    snprintf(filename, sizeof(filename), "%s.txt", accountNumber);
    FILE* file = fopen(filename, "r+");
    if (file) {
        double currentBalance = 0.0;
        fscanf(file, "%lf", &currentBalance);
        fseek(file, 0, SEEK_SET);
        fprintf(file, "%.2f\n", currentBalance + amount);
        fclose(file);
    }
}

void withdraw(const char* accountNumber, double amount) {
    char filename[255];
    snprintf(filename, sizeof(filename), "%s.txt", accountNumber);
    FILE* file = fopen(filename, "r+");
    if (file) {
        double currentBalance;
        fscanf(file, "%lf", &currentBalance);
        if (currentBalance >= amount) {
            fseek(file, 0, SEEK_SET);
            fprintf(file, "%.2f\n", currentBalance - amount);
        }
        fclose(file);
    }
}

void transfer(const char* fromAccount, const char* toAccount, double amount) {
    double fromBalance = inquiry(fromAccount);
    if (fromBalance >= amount) {
        withdraw(fromAccount, amount);
        deposit(toAccount, amount);
    }
}

double inquiry(const char* accountNumber) {
    char filename[255];
    snprintf(filename, sizeof(filename), "%s.txt", accountNumber);
    FILE* file = fopen(filename, "r");
    double balance = 0.0;
    if (file) {
        fscanf(file, "%lf", &balance);
        fclose(file);
    }
    return balance;
}

void close_account(const char* accountNumber) {
    char filename[255];
    snprintf(filename, sizeof(filename), "%s.txt", accountNumber);
    remove(filename);
}

void process_transaction(SharedTransaction* transactions, int index) {
    SharedTransaction* txn = &transactions[index];
    double balance;

    switch (txn->type) {
        case CREATE:
            create_account(txn->accountNumber, txn->amount);
            strcpy(txn->status, "success");
            break;
        case DEPOSIT:
            deposit(txn->accountNumber, txn->amount);
            strcpy(txn->status, "success");
            break;
        case WITHDRAW:
            balance = inquiry(txn->accountNumber);
            if (balance >= txn->amount) {
                withdraw(txn->accountNumber, txn->amount);
                strcpy(txn->status, "success");
            } else {
                strcpy(txn->status, "failed");
            }
            break;
        case TRANSFER:
            balance = inquiry(txn->accountNumber);
            if (balance >= txn->amount) {
                transfer(txn->accountNumber, txn->recipientAccountNumber, txn->amount);
                strcpy(txn->status, "success");
            } else {
                strcpy(txn->status, "failed");
            }
            break;
        case INQUIRY:
            balance = inquiry(txn->accountNumber);
            // For INQUIRY, you might want to log the balance or handle it differently
            strcpy(txn->status, "success");
            break;
        case CLOSE:
            close_account(txn->accountNumber);
            strcpy(txn->status, "success");
            break;
        default:
            strcpy(txn->status, "failed");
            break;
    }

    // Timestamp the transaction
    time_t now = time(NULL);
    strftime(txn->timestamp, 20, "%Y-%m-%d %H:%M:%S", localtime(&now));
    txn->processed = 1; // Mark the transaction as processed
}
void log_transaction(const SharedTransaction* txn) {
    printf("Transaction: %d | Account: %s | Amount: %.2f | Status: %s | Timestamp: %s\n",
           txn->type, txn->accountNumber, txn->amount, txn->status, txn->timestamp);
}
