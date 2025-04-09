#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <string.h>
#include "common.h"

int main()
{
    mqd_t mq;
    char buffer[MAX_SIZE];

    // Open the message queue for reading
    mq = mq_open(QUEUE_NAME, O_RDONLY, 0644, NULL);
    if (mq == -1)
    {
        perror("mq_open");
        return -1;
    }

    printf("Waiting for messages...\n");
    while (1)
    {
        ssize_t bytes = mq_receive(mq, buffer, MAX_SIZE, NULL);
        if (bytes >= 0)
        {
            buffer[bytes] = '\0';
            printf("Received: %s\n", buffer);
            if (strcmp(buffer, "exit") == 0)
                break;
        }
        else
        {
            perror("mq_receive");
            break;
        }
    }
    mq_close(mq);
    mq_unlink(QUEUE_NAME);
    return 0;
}
