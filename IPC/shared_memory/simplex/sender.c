#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define SHM_NAME   "/shm_simplex"
#define SHM_SIZE   4096

int main()
{
    int shm_fd; 

    shm_fd = shm_open(SHM_NAME, O_RDWR|O_CREAT, 0666);
    if (shm_fd == -1)
    {
        perror("shm_open");
        exit(1);

    }

    if (ftruncate(shm_fd, SHM_SIZE) == -1)
    {
        perror("ftruncate");
        exit(1);
    }

    void* ptr = mmap(NULL, SHM_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == NULL)
    {
        perror("mmap");
        exit(1);
    }

    const char* message = "Hello World";
    memcpy(ptr, (void* )message, strlen(message) + 1);

    printf("Message written to the shared memory\n");

    munmap(ptr, SHM_SIZE);
    
    return 0;

}