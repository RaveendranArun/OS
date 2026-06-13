#include <stdio.h>
#include <pthread.h>

pthread_mutex_t rmtx;

void* foobar(void* arg)
{
    pthread_mutex_lock(&rmtx);
    pthread_mutex_lock(&rmtx);
    printf("in thread\n");

    pthread_mutex_unlock(&rmtx);
    pthread_mutex_unlock(&rmtx);

    return NULL;
}

int main()
{
    
    pthread_mutexattr_t rmtxattr;
    pthread_mutexattr_init(&rmtxattr);
    pthread_mutexattr_settype(&rmtxattr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutexattr_destroy(&rmtxattr);
    
    pthread_mutex_init(&rmtx, &rmtxattr);

    pthread_t tid;
    
    pthread_create(&tid, NULL, foobar, NULL);
    pthread_join(tid, NULL);
    return 0;
}