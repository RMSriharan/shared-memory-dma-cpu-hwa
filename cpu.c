// cpu.c
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#define SHM_NAME "/cpu_hwa_shared"
#define SIZE 4096
#define DATA_SIZE 2048

typedef struct {
    pthread_mutex_t lock;
    int flag;
    char buffer[DATA_SIZE];
} SharedData;

int main() {
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    ftruncate(shm_fd, SIZE);

    SharedData *shared = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&shared->lock, &attr);

    pthread_mutex_lock(&shared->lock);

    printf("[CPU] Writing DMA-like large data to shared memory...\n");
    for (int i = 0; i < DATA_SIZE; i++) {
        shared->buffer[i] = 'A' + (i % 26);
    }

    shared->flag = 1;
    msync(shared, SIZE, MS_SYNC);
    pthread_mutex_unlock(&shared->lock);

    printf("[CPU] Waiting for HWA to process...\n");
    while (shared->flag != 2);

    pthread_mutex_lock(&shared->lock);
    printf("[CPU] HWA response (preview): %.50s...\n", shared->buffer);
    pthread_mutex_unlock(&shared->lock);

    munmap(shared, SIZE);
    close(shm_fd);
    shm_unlink(SHM_NAME);
    return 0;
}

