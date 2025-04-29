#include "fifo_shared_mem.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int main()
{
    uint8_t recv_buff[BUFFER_SIZE];

    int shm_fd = shm_open(SHM_NAME, O_CREAT|O_RDWR, 0666);
    if (shm_fd == -1)
    {
        perror("shm_open");
        exit(1);
    }

    if (ftruncate(shm_fd, sizeof(shared_memory_t)) == -1)
    {
        perror("ftruncate");
        exit(1);
    }

    shared_memory_t* pshm = (shared_memory_t* )mmap(NULL, sizeof(shared_memory_t), PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0 );
    if (pshm == NULL)
    {
        perror("mmap");
        exit(1);
    }
    
    while (1)
    {
        int n = fifo_read(&pshm->shm_a2b_fifo, recv_buff, BUFFER_SIZE);
        if (n > 0)
        {
            printf("[B] received from A: %s\n", recv_buff);
        }


        const char* message = "Hello from B!";
        if (fifo_write(&pshm->shm_b2a_fifo, (uint8_t* )message, (uint16_t)(strlen(message) + 1)) == 0)
        {
            printf("[B] sent: %s\n", message);
        }

       

        usleep(500000);
    }

    return 0;
}