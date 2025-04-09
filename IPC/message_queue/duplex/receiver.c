#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"

int main()
{
    mqd_t send_q, recv_q;
    char buffer[MAX_SIZE];
    struct mq_attr attr = {0, 10, MAX_SIZE, 0};


    // Receive from A, send to A;
    recv_q = mq_open(QUEUE_A_TO_B, O_CREAT | O_RDONLY, 0644, &attr);
    send_q = mq_open(QUEUE_B_TO_A, O_CREAT | O_WRONLY, 0644, &attr);
    if (send_q == -1 || recv_q == -1)
    {
        perror("mq_open");
        return -1;
    }

    printf("Process B ready\n");
    while (1)
    {
        ssize_t bytes = mq_receive(recv_q, buffer, MAX_SIZE, NULL);
        if (bytes >= 0)
        {
            buffer[bytes] = '\0';
            printf("A sent: %s\n", buffer);
            if (strcmp(buffer, "exit") == 0) break;

            // Reply
            printf("B: ");
            fgets(buffer, MAX_SIZE, stdin);
            buffer[strcspn(buffer, "\n")] = '\0';
            mq_send(send_q, buffer, strlen(buffer) +1, 0);
        }
    }

    mq_close(send_q);
    mq_close(recv_q);
    mq_unlink(QUEUE_A_TO_B);
    mq_unlink(QUEUE_B_TO_A);
    return 0;
}
