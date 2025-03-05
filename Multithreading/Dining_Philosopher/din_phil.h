#ifndef __DIN_PHIL_H__
#define __DIN_PHIL_H__

#include <stdint.h>
#include <pthread.h>

typedef struct phil_
{
    uint32_t id;
    pthread_t thread_handle;
    uint32_t eat_count;
}phil_t;

typedef struct spoon_
{
    uint32_t spoon_id;
    bool is_used;           // To indicate, if the spoon is being used or not 
    phil_t* phil;           // If used, then which philosopher is using it
    pthread_mutex_t mutex;  // For mutual exclusion
    pthread_cond_t cv;      // Fro thread coordination, competing for this resource.
}spoon_t;


#endif // __DIN_PHIL_H__


