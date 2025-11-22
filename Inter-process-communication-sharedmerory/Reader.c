// consumer.c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <semaphore.h>
#include <errno.h>

#define SHM_NAME "/posix_shm_demo"
#define SEM_EMPTY_NAME "/posix_sem_empty"
#define SEM_FULL_NAME  "/posix_sem_full"
#define SEM_MUTEX_NAME "/posix_sem_mutex"

#define BUF_SIZE 8
#define ITEM_SIZE 256

typedef struct {
    int head;
    int tail;
    char buf[BUF_SIZE][ITEM_SIZE];
} shm_region_t;

int main(void) {
    // Open existing shared memory
    int shm_fd;
    shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    while (shm_fd == -1 && errno == ENOENT) {
        // Wait until producer creates it
        fprintf(stderr, "Waiting for shared memory to be created...\n");
        sleep(1);
        shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    }
    if (shm_fd == -1) { perror("shm_open"); return 1; }

    size_t shm_sz = sizeof(shm_region_t);
    shm_region_t *shm = mmap(NULL, shm_sz, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm == MAP_FAILED) { perror("mmap"); close(shm_fd); return 1; }

    // Open semaphores (create if missing to avoid blocking forever; creation does not reinit)
    sem_t *sem_empty = sem_open(SEM_EMPTY_NAME, 0);
    while (sem_empty == SEM_FAILED && errno == ENOENT) {
        fprintf(stderr, "Waiting for semaphores to be created...\n");
        sleep(1);
        sem_empty = sem_open(SEM_EMPTY_NAME, 0);
    }
    if (sem_empty == SEM_FAILED) { perror("sem_open empty"); goto cleanup; }

    sem_t *sem_full = sem_open(SEM_FULL_NAME, 0);
    while (sem_full == SEM_FAILED && errno == ENOENT) {
        fprintf(stderr, "Waiting for semaphores to be created...\n");
        sleep(1);
        sem_full = sem_open(SEM_FULL_NAME, 0);
    }
    if (sem_full == SEM_FAILED) { perror("sem_open full"); goto cleanup; }

    sem_t *sem_mutex = sem_open(SEM_MUTEX_NAME, 0);
    while (sem_mutex == SEM_FAILED && errno == ENOENT) {
        fprintf(stderr, "Waiting for semaphores to be created...\n");
        sleep(1);
        sem_mutex = sem_open(SEM_MUTEX_NAME, 0);
    }
    if (sem_mutex == SEM_FAILED) { perror("sem_open mutex"); goto cleanup; }

    printf("Consumer ready. Press Ctrl+C to exit.\n");

    for (;;) {
        // wait for full slot
        if (sem_wait(sem_full) == -1) {
            if (errno == EINTR) continue;
            perror("sem_wait full");
            break;
        }

        // enter critical section
        if (sem_wait(sem_mutex) == -1) {
            perror("sem_wait mutex");
            sem_post(sem_full);
            break;
        }

        // read from buffer[head]
        char item[ITEM_SIZE];
        strncpy(item, shm->buf[shm->head], ITEM_SIZE - 1);
        item[ITEM_SIZE - 1] = '\0';
        shm->head = (shm->head + 1) % BUF_SIZE;

        // leave critical section
        if (sem_post(sem_mutex) == -1) { perror("sem_post mutex"); break; }

        // signal empty slot
        if (sem_post(sem_empty) == -1) { perror("sem_post empty"); break; }

        // process the item (print)
        printf("Consumer received: %s\n", item);
        fflush(stdout);
    }

cleanup:
    if (shm != MAP_FAILED) munmap(shm, shm_sz);
    close(shm_fd);
    return 0;
}
