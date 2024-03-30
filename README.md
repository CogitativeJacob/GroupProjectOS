# Project Architecture Document for Synchronized Banking System

## Architectural Overview

The Synchronized Banking System is designed to manage banking transactions in a concurrent environment, leveraging the power of process-based parallelism and synchronization techniques in C. The system focuses on ensuring data consistency and integrity through controlled access to shared resources, such as transaction logs.

## Component Design

### Shared Memory (`shared_memory.h` & `shared_memory.c`)

**Purpose:** Facilitates inter-process communication (IPC) by providing a shared space for processes to read and write transaction data.

- **Key Functions:**
  - `initSharedMemory(size_t size)`: Initializes a shared memory segment.
  - `attachSharedMemory(int shmid, int shmflg)`: Attaches to the shared memory segment.
  - `detachSharedMemory(const void *shmaddr)`: Detaches from the shared memory segment.
  - `cleanupSharedMemory(int shmid)`: Removes the shared memory segment.

### Process Management

**Included in:** Main driver file (`main.c`)

**Purpose:** Manages the lifecycle of child processes that handle individual transactions to leverage parallelism.

- **Implementation:** Utilizes the `fork()` system call to create child processes. Each child process is responsible for processing a single transaction or a batch of transactions.

### Transactions (`transactions.h` & `transactions.c`)

**Purpose:** Defines the structure of transactions and provides functionality to process and log transactions.

- **Data Structures:**
  - `Transaction`: Contains details about a transaction, including type, account number, amount, and timestamp.
- **Key Functions:**
  - `appendTransactionToFile(Transaction* transaction)`: Appends a transaction record to the transaction log file.

### Synchronization (`synchronization.h` & `synchronization.c`)

**Purpose:** Ensures that access to shared resources, such as the transaction log file, is synchronized among processes to prevent data corruption and race conditions.

- **Data Structures:**
  - `MonitorQueue`: A queue that manages access to a shared resource through a monitor.
  - `QueueNode`: Represents a process waiting for access to the shared resource.
- **Key Functions:**
  - `enterMonitor(MonitorQueue* q, Transaction* t)`: Process enters the monitor, waiting its turn to access the shared resource.
  - `exitMonitor(MonitorQueue* q)`: Process exits the monitor, signaling the next in line.

### Utility Functions (`utils.h`)

**Purpose:** Provides additional helper functions as needed for file operations, error handling, and logging.

## Synchronization Strategy

The system utilizes a monitor construct, encapsulated by the `MonitorQueue`, to control access to shared resources. This strategy prevents race conditions and ensures that only one process at a time can perform operations on the shared resource, thereby maintaining data integrity.

- Processes attempting to access the shared resource are enqueued in the `MonitorQueue`.
- The `enterMonitor` function locks the shared resource, checks if the process is at the front of the queue, and waits if not.
- Once a process has completed its operation on the shared resource, it calls `exitMonitor` to remove itself from the queue and signal the next waiting process.

## IPC Mechanisms

Inter-process communication is achieved through shared memory, allowing child processes to share transaction data with each other and with the parent process. This mechanism is critical for coordinating actions and sharing status updates among processes.

## Conclusion

This architecture provides a robust foundation for building a synchronized banking system capable of handling concurrent transactions safely and efficiently. Future enhancements may include more complex transaction processing logic, enhanced error handling, and expanded synchronization mechanisms to support additional shared resources.
