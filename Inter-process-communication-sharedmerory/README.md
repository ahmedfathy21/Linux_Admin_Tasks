# Inter-Process Communication using Shared Memory

A comprehensive demonstration of **POSIX shared memory** and **semaphore-based synchronization** implementing the classic **producer-consumer problem** in C on Linux.

## 📋 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [System Requirements](#system-requirements)
- [Project Structure](#project-structure)
- [How It Works](#how-it-works)
- [Compilation](#compilation)
- [Usage](#usage)
- [Technical Details](#technical-details)
- [Cleanup](#cleanup)
- [Learning Objectives](#learning-objectives)

## 🎯 Overview

This project demonstrates inter-process communication (IPC) using POSIX shared memory with three synchronized processes:

- **Producer (writer)**: Writes data into a shared circular buffer
- **Consumer (reader)**: Reads and processes data from the shared buffer
- **Cleanup utility**: Removes IPC resources (shared memory and semaphores)

The implementation uses **semaphores** to solve the classic synchronization problem, ensuring thread-safe access to shared resources and preventing race conditions.

## ✨ Features

- ✅ POSIX shared memory (`shm_open`, `mmap`)
- ✅ Named POSIX semaphores for synchronization
- ✅ Circular buffer implementation (8 slots, 256 bytes each)
- ✅ Producer-consumer pattern with mutual exclusion
- ✅ Graceful error handling
- ✅ Resource cleanup utility
- ✅ Support for multiple concurrent processes

## 🔧 System Requirements

- **OS**: Linux (POSIX-compliant)
- **Compiler**: GCC (with C11 support)
- **Libraries**: 
  - pthread (linking with `-lpthread` for semaphores)
  - rt (real-time extensions, `-lrt`)

## 📁 Project Structure

```
Inter-process-communication-sharedmerory/
├── writer.c          # Producer process (writes to shared memory)
├── Reader.c          # Consumer process (reads from shared memory)
├── Cleanup.c         # Utility to clean up IPC resources
├── producer          # Compiled producer binary
├── consumer          # Compiled consumer binary
├── cleanup           # Compiled cleanup binary
├── shmfile           # Sample shared memory file (if applicable)
└── README.md         # This file
```

## 🔄 How It Works

### Architecture

```
┌─────────────┐                    ┌─────────────┐
│  Producer   │                    │  Consumer   │
│  (writer)   │                    │  (reader)   │
└──────┬──────┘                    └──────┬──────┘
       │                                  │
       │    ┌────────────────────┐       │
       └───►│  Shared Memory     │◄──────┘
            │  Circular Buffer   │
            │  (8 slots × 256B)  │
            └────────────────────┘
                     ▲
                     │
            ┌────────┴────────┐
            │   Semaphores    │
            │  • empty (8)    │
            │  • full (0)     │
            │  • mutex (1)    │
            └─────────────────┘
```

### Synchronization Mechanism

1. **sem_empty**: Counts available empty slots (initialized to `BUF_SIZE = 8`)
2. **sem_full**: Counts available full slots (initialized to `0`)
3. **sem_mutex**: Ensures mutual exclusion for critical section access (binary semaphore)

### Producer Logic

1. Wait for an empty slot (`sem_wait(empty)`)
2. Acquire mutex lock (`sem_wait(mutex)`)
3. Write data to `buffer[tail]`
4. Update `tail = (tail + 1) % BUF_SIZE`
5. Release mutex (`sem_post(mutex)`)
6. Signal full slot available (`sem_post(full)`)

### Consumer Logic

1. Wait for a full slot (`sem_wait(full)`)
2. Acquire mutex lock (`sem_wait(mutex)`)
3. Read data from `buffer[head]`
4. Update `head = (head + 1) % BUF_SIZE`
5. Release mutex (`sem_post(mutex)`)
6. Signal empty slot available (`sem_post(empty)`)

## 🔨 Compilation

Compile all three programs using GCC:

```bash
# Compile producer
gcc -o producer writer.c -lpthread -lrt

# Compile consumer
gcc -o consumer Reader.c -lpthread -lrt

# Compile cleanup utility
gcc -o cleanup Cleanup.c -lpthread -lrt
```

Or use a one-liner to compile all:

```bash
gcc -o producer writer.c -lpthread -lrt && \
gcc -o consumer Reader.c -lpthread -lrt && \
gcc -o cleanup Cleanup.c -lpthread -lrt
```

## 🚀 Usage

### Step 1: Start the Producer

Open a terminal and run:

```bash
./producer
```

You'll see:
```
Producer ready. Type lines (max 255 chars). Ctrl+D to exit.
```

Type messages and press Enter. Each message will be placed in the shared buffer.

### Step 2: Start the Consumer

In another terminal, run:

```bash
./consumer
```

You'll see:
```
Consumer ready. Press Ctrl+C to exit.
Consumer received: <your message>
```

The consumer will continuously read and display messages from the shared buffer.

### Step 3: Clean Up Resources

After stopping both processes (Ctrl+D for producer, Ctrl+C for consumer), clean up IPC resources:

```bash
./cleanup
```

Expected output:
```
Unlinked shared memory /posix_shm_demo
Unlinked semaphore /posix_sem_empty
Unlinked semaphore /posix_sem_full
Unlinked semaphore /posix_sem_mutex
```

## 🔬 Technical Details

### Shared Memory Structure

```c
typedef struct {
    int head;                        // Consumer read position
    int tail;                        // Producer write position
    char buf[BUF_SIZE][ITEM_SIZE];  // Circular buffer (8 × 256 bytes)
} shm_region_t;
```

### IPC Resource Names

- **Shared Memory**: `/posix_shm_demo`
- **Semaphores**:
  - `/posix_sem_empty` - Tracks empty slots
  - `/posix_sem_full` - Tracks full slots
  - `/posix_sem_mutex` - Mutual exclusion lock

### Buffer Configuration

- **BUF_SIZE**: 8 slots
- **ITEM_SIZE**: 256 bytes per slot
- **Total shared memory**: ~2 KB

### Key POSIX Functions Used

| Function | Purpose |
|----------|---------|
| `shm_open()` | Create/open shared memory object |
| `ftruncate()` | Set shared memory size |
| `mmap()` | Map shared memory into process address space |
| `munmap()` | Unmap shared memory |
| `shm_unlink()` | Remove shared memory object |
| `sem_open()` | Create/open named semaphore |
| `sem_wait()` | Decrement semaphore (blocking) |
| `sem_post()` | Increment semaphore |
| `sem_unlink()` | Remove named semaphore |

## 🧹 Cleanup

**Important**: Always run `./cleanup` after finishing to avoid leaving orphaned IPC resources.

To manually check for lingering resources:

```bash
# List shared memory objects
ls -la /dev/shm/

# List semaphores
ls -la /dev/shm/sem.*
```

To manually remove (if cleanup fails):

```bash
rm /dev/shm/posix_shm_demo
rm /dev/shm/sem.posix_sem_*
```

## 📚 Learning Objectives

This project demonstrates:

1. ✅ **POSIX Shared Memory**: Efficient zero-copy IPC mechanism
2. ✅ **Semaphore Synchronization**: Classical solution to producer-consumer problem
3. ✅ **Critical Sections**: Protecting shared data with mutexes
4. ✅ **Circular Buffers**: Efficient bounded-buffer implementation
5. ✅ **Error Handling**: Robust system call error checking
6. ✅ **Resource Management**: Proper creation and cleanup of IPC resources
7. ✅ **Concurrent Programming**: Multiple processes accessing shared resources safely

## 🐛 Troubleshooting

### Consumer waits indefinitely

- Ensure producer is running first to create shared memory and semaphores
- The consumer will retry connection every second until resources are available

### "Address already in use" errors

- Run `./cleanup` to remove stale IPC resources
- Check `/dev/shm/` for orphaned objects

### Permission denied

- Ensure you have permissions to create IPC objects
- Both processes should run under the same user

## 📝 License

This project is created for educational purposes as part of Linux System Administration coursework.

## 👤 Author

Linux Administration Task - Inter-Process Communication Demo

---

**Note**: This implementation is for learning purposes. Production systems should include additional error handling, signal management, and resource limits.
