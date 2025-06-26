// hwa.c
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
    int shm_fd;

    // Wait until CPU creates shared memory
    while ((shm_fd = shm_open(SHM_NAME, O_RDWR, 0666)) == -1) {
        printf("[HWA] Waiting for shared memory to be created...\n");
        sleep(1);
    }

    SharedData *shared = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    printf("[HWA] Waiting for CPU to write data...\n");
    while (shared->flag != 1);

    pthread_mutex_lock(&shared->lock);

    printf("[HWA] Processing data from CPU...\n");
    for (int i = 0; i < DATA_SIZE; i++) {
        shared->buffer[i] = shared->buffer[i] + 1; // Simple transformation
    }

    shared->flag = 2;
    msync(shared, SIZE, MS_SYNC);
    pthread_mutex_unlock(&shared->lock);

    munmap(shared, SIZE);
    close(shm_fd);
    return 0;
}

