// producer.c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>      // O_* constants
#include <sys/mman.h>
#include <sys/stat.h>   // mode constants
#include <unistd.h>
#include <semaphore.h>
#include <errno.h>

#define SHM_NAME "/posix_shm_demo"
#define SEM_EMPTY_NAME "/posix_sem_empty"
#define SEM_FULL_NAME  "/posix_sem_full"
#define SEM_MUTEX_NAME "/posix_sem_mutex"

#define BUF_SIZE 8            // number of slots
#define ITEM_SIZE 256         // bytes per slot

typedef struct {
    int head;
    int tail;
    char buf[BUF_SIZE][ITEM_SIZE];
} shm_region_t;

int main(int argc, char **argv) {
    (void) argc;
    (void) argv;

    int shm_fd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        return 1;
    }

    size_t shm_sz = sizeof(shm_region_t);
    if (ftruncate(shm_fd, shm_sz) == -1) {
        perror("ftruncate");
        close(shm_fd);
        return 1;
    }

    shm_region_t *shm = mmap(NULL, shm_sz, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm == MAP_FAILED) {
        perror("mmap");
        close(shm_fd);
        return 1;
    }

    // Initialize shared region (only first creator should do this)
    // We attempt to zero-init always; if another process already created it,
    // the initialization may overwrite — in production you'd check ownership.
    shm->head = 0;
    shm->tail = 0;
    for (int i = 0; i < BUF_SIZE; ++i) shm->buf[i][0] = '\0';

    // Create or open semaphores
    sem_t *sem_empty = sem_open(SEM_EMPTY_NAME, O_CREAT, 0666, BUF_SIZE);
    if (sem_empty == SEM_FAILED) { perror("sem_open empty"); goto cleanup; }

    sem_t *sem_full = sem_open(SEM_FULL_NAME, O_CREAT, 0666, 0);
    if (sem_full == SEM_FAILED) { perror("sem_open full"); goto cleanup; }

    sem_t *sem_mutex = sem_open(SEM_MUTEX_NAME, O_CREAT, 0666, 1);
    if (sem_mutex == SEM_FAILED) { perror("sem_open mutex"); goto cleanup; }

    printf("Producer ready. Type lines (max %d chars). Ctrl+D to exit.\n", ITEM_SIZE - 1);

    char line[ITEM_SIZE];
    while (fgets(line, sizeof(line), stdin) != NULL) {
        // trim newline
        size_t L = strlen(line);
        if (L && line[L-1] == '\n') line[L-1] = '\0';

        // wait for empty slot
        if (sem_wait(sem_empty) == -1) {
            if (errno == EINTR) continue;
            perror("sem_wait empty");
            break;
        }

        // enter critical section
        if (sem_wait(sem_mutex) == -1) {
            perror("sem_wait mutex");
            sem_post(sem_empty); // restore
            break;
        }

        // write into buffer[tail]
        strncpy(shm->buf[shm->tail], line, ITEM_SIZE - 1);
        shm->buf[shm->tail][ITEM_SIZE - 1] = '\0';
        shm->tail = (shm->tail + 1) % BUF_SIZE;

        // leave critical section
        if (sem_post(sem_mutex) == -1) { perror("sem_post mutex"); break; }

        // signal that full slot available
        if (sem_post(sem_full) == -1) { perror("sem_post full"); break; }
    }

    printf("Producer exiting.\n");

cleanup:
    if (shm != MAP_FAILED) munmap(shm, shm_sz);
    close(shm_fd);
    // keep semaphores and shm object for consumer; do not unlink here
    return 0;
}
