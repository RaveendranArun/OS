#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <unistd.h>

#include "din_phil.h"

#define N_PHILOSOPHER    5
#define N_SPOONS         N_PHILOSOPHER

static phil_t phil[N_PHILOSOPHER];
static spoon_t spoon[N_SPOONS];

static spoon_t* phil_get_right_spoon(phil_t* phil)
{
    int phil_id = phil->id;
    
    if (phil_id == 0)
    {
        return &spoon[N_SPOONS-1];
    }

    return &spoon[phil_id-1];
}

static spoon_t* phil_get_left_spoon(phil_t* phil)
{
    return &spoon[phil->id];
}

static void phil_eat(phil_t* phil)
{
    spoon_t* left_spoon = phil_get_left_spoon(phil);
    spoon_t* right_spoon = phil_get_right_spoon(phil);

    /*
     * Check condition that philosopher is eating with right set of spoons
     */
    assert(left_spoon->phil == phil);
    assert(right_spoon->phil == phil);
    assert(left_spoon->is_used == true);
    assert(right_spoon->is_used == true);
    phil->eat_count++;
    printf("phil %d eats with spoon [%d %d] for %d times\n", phil->id, left_spoon->spoon_id, right_spoon->spoon_id, phil->eat_count);
    sleep(1); // left the philosopher eats the cake for 1 sec
}

static void philosopher_release_both_spoons(phil_t* phil)
{
    spoon_t* left_spoon = phil_get_left_spoon(phil);
    spoon_t* right_spoon = phil_get_right_spoon(phil);

    pthread_mutex_lock(&left_spoon->mutex);
    pthread_mutex_lock(&right_spoon->mutex);

    assert(left_spoon->phil == phil);
    assert(left_spoon->is_used == true);

    assert(right_spoon->phil == phil);
    assert(right_spoon->is_used == true);
    pthread_mutex_unlock(&right_spoon->mutex);
    pthread_mutex_unlock(&left_spoon->mutex);

    pthread_mutex_lock(&left_spoon->mutex);
    printf("phil %d releasing the left spoon %d\n", phil->id, left_spoon->spoon_id);
    left_spoon->phil = NULL;
    left_spoon->is_used = false;

    pthread_cond_signal(&left_spoon->cv);
    printf("phil %d signalled the phil waiting for the left spoon %d\n", phil->id, left_spoon->spoon_id);
    pthread_mutex_unlock(&left_spoon->mutex);

    pthread_mutex_lock(&right_spoon->mutex);
    printf("phil %d releasing the right spoon %d\n", phil->id, right_spoon->spoon_id);
    right_spoon->phil = NULL;
    right_spoon->is_used = false;
    
    pthread_cond_signal(&right_spoon->cv);
    printf("phil %d signalled the phil waiting for the right spoon %d\n", phil->id, right_spoon->spoon_id);
    pthread_mutex_unlock(&right_spoon->mutex);
}

static bool philosopher_get_access_both_spoons(phil_t* phil)
{
    spoon_t* left_spoon = phil_get_left_spoon(phil);
    spoon_t* right_spoon = phil_get_right_spoon(phil);

    /**
     * Before checking the status of the spoon, lock it, while one
     * philosopher is inspecting the state of the spoon, no other phil must change it.
     */

    printf("phil %d waiting for lock on left spoon %d.\n", phil->id, left_spoon->spoon_id);
    pthread_mutex_lock(&left_spoon->mutex);
    printf("phil %d inspecting the left spoon %d state.\n", phil->id, left_spoon->spoon_id);

    //case 1: Spoon is used by someother phil, then wait
    while (left_spoon->is_used && (left_spoon->phil != phil))
    {
        printf("phil %d blocks as left spoon %d is not available\n", phil->id, left_spoon->spoon_id);
        pthread_cond_wait(&left_spoon->cv, &left_spoon->mutex);
        printf("phil %d receives the signal to grab the spoon %d\n", phil->id, left_spoon->spoon_id);
    }

    // case 2: Spoon is available, grab it and try for another spoon

    printf("phil %d finds left spoon %d available, trying to grab it\n", phil->id, left_spoon->spoon_id);
    left_spoon->is_used = true;
    left_spoon->phil = phil;
    printf("phil %d has successfully grab the left spoon %d\n", phil->id, left_spoon->spoon_id);
    pthread_mutex_unlock(&left_spoon->mutex);

    // case 3: Trying to grab the right spoon now
    printf("phil %d making an attempt to grab the right spoon %d", phil->id, right_spoon->spoon_id);
    // lock the right spoon before inspecting its state
    printf("phil %d waiting for lock on right spoon %d\n", phil->id, right_spoon->spoon_id);
    pthread_mutex_lock(&right_spoon->mutex);
    printf("phil %d inspecting the right spoon state %d\n", phil->id, right_spoon->spoon_id);

    if (right_spoon->is_used == false)
    {
        // right spoon is available, grab it and eat
        right_spoon->is_used = true;
        right_spoon->phil = phil;
        pthread_mutex_unlock(&right_spoon->mutex);
        return true;
    }
    else if (right_spoon->is_used == true)
    {
        if (right_spoon->phil != phil)
        {
            printf("phil %d finds right spoon %d is already used by phil %d. Releasing the left spoon as well\n", phil->id, right_spoon->spoon_id, right_spoon->phil->id);
            pthread_mutex_lock(&left_spoon->mutex);
            assert(left_spoon->is_used == true);
            assert(left_spoon->phil == phil);
            left_spoon->is_used = false;
            left_spoon->phil = NULL;
            printf("phil %d releases the left spoon %d\n", phil->id, left_spoon->spoon_id);
            pthread_mutex_unlock(&left_spoon->mutex);
            pthread_mutex_unlock(&right_spoon->mutex);
            return false;
        }
        else
        {
            printf("phil %d is already using the right spoon %d\n", phil->id, right_spoon->spoon_id);
            pthread_mutex_unlock(&right_spoon->mutex);
            return true;
        }
        
    }

    assert(0); // should be dead code
    return false; 
}

void* philosopher_fn(void* arg)
{
    phil_t* phil = (phil_t* )arg;
    
    while (1)
    {
        if (philosopher_get_access_both_spoons(phil))
        {
            phil_eat(phil);
            philosopher_release_both_spoons(phil);
            sleep(1);
        }
    }
}

int main()
{
    int i;
    
    // Initilize the spoons
    for (i = 0; i < N_SPOONS; ++i)
    {
        spoon[i].spoon_id = i;
        spoon[i].is_used = false;
        spoon[i].phil = NULL;
        pthread_mutex_init(&spoon[i].mutex, NULL);
        pthread_cond_init(&spoon[i].cv, NULL);
    }

    // Create philosopher threads
    for (i = 0 ; i < N_PHILOSOPHER; ++i)
    {
        phil[i].id = i;
        phil[i].eat_count = 0;
        pthread_create(&phil[i].thread_handle, NULL, philosopher_fn, &phil[i]);
    }

    for (i = 0; i < N_PHILOSOPHER; ++i)
    {
        pthread_join(phil[i].thread_handle, NULL);
    }

    return 0;


}