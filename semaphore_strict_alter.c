#include <stdio.h>
#include "sema.h"

sema_t* sem_odd = NULL;
sema_t* sem_eve = NULL;

int counter = 1;

void* thread_even(void* arg)
{
    while (counter < 15)
    {
        sema_wait(sem_odd);
        printf("%d ", counter++);
        sema_post(sem_eve);
    }
}

void* thread_odd(void* arg)
{
    while (counter < 15)
    {
        printf("%d ", counter++);
        sema_post(sem_odd);
        sema_wait(sem_eve);
    }
}

int main()
{
    pthread_t tid[2];

    sem_odd = sema_get_new_semaphore();
    sem_eve = sema_get_new_semaphore();
    sema_init(sem_odd, 0);
    sema_init(sem_eve, 0);

    pthread_create(&tid[0], NULL, thread_even, NULL);
    pthread_create(&tid[1], NULL, thread_odd, NULL);

    for (int i = 0; i < 2; ++i)
        pthread_join(tid[i], NULL);
    printf("\n");
    sema_destroy(sem_odd);
    sema_destroy(sem_eve);
    return 0;
}
