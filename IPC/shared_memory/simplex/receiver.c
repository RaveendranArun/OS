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

    shm_fd = shm_open(SHM_NAME, O_RDONLY, 0666);
    if (shm_fd == -1)
    {
        perror("shm_open");
        exit(1);

    }

    void* ptr = mmap(NULL, SHM_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (ptr == NULL)
    {
        perror("mmap");
        exit(1);
    }

    printf("Reader: Read message-> %s\n", (char* )ptr);


    munmap(ptr, SHM_SIZE);
    close(shm_fd);

    shm_unlink(SHM_NAME);

    return 0;

}