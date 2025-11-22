// cleanup.c
#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define SHM_NAME "/posix_shm_demo"
#define SEM_EMPTY_NAME "/posix_sem_empty"
#define SEM_FULL_NAME  "/posix_sem_full"
#define SEM_MUTEX_NAME "/posix_sem_mutex"

int main(void) {
    if (shm_unlink(SHM_NAME) == 0) {
        printf("Unlinked shared memory %s\n", SHM_NAME);
    } else {
        perror("shm_unlink");
    }

    if (sem_unlink(SEM_EMPTY_NAME) == 0) {
        printf("Unlinked semaphore %s\n", SEM_EMPTY_NAME);
    } else {
        perror("sem_unlink empty");
    }

    if (sem_unlink(SEM_FULL_NAME) == 0) {
        printf("Unlinked semaphore %s\n", SEM_FULL_NAME);
    } else {
        perror("sem_unlink full");
    }

    if (sem_unlink(SEM_MUTEX_NAME) == 0) {
        printf("Unlinked semaphore %s\n", SEM_MUTEX_NAME);
    } else {
        perror("sem_unlink mutex");
    }

    return 0;
}
