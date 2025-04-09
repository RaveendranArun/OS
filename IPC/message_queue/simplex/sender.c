#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <string.h>
#include "common.h"

int main()
{
    mqd_t mq;
    struct mq_attr attr = {0, 10, MAX_SIZE, 0};
    char buffer[MAX_SIZE];

    //create message queue for writing
    mq = mq_open(QUEUE_NAME, O_CREAT | O_WRONLY, 0644, &attr);
    if (mq == -1)
    {
        perror("mq_open");
        return -1;
    }

    printf("Enter message (type 'exit' to quit)\n");
    while (1)
    {
        fgets(buffer, MAX_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        if (mq_send(mq, buffer, strlen(buffer)+1, 0) == -1)
        {
            perror("mq_send");
            break;;
        }

        if (strcmp(buffer, "exit") == 0)
        {
            break;
        }
    }
    mq_close(mq);
    return 0;
}
