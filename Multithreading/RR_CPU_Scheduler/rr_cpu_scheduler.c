#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <errno.h>

#define MAX_NUM_PROCESS  20
#define MAX_TIME_QUANTUM 3

typedef struct {
    uint32_t id;
    uint32_t burst_time;
    uint32_t remaining_time;
    bool is_finished;
    pthread_t thread_id;
} Process;

Process process[MAX_NUM_PROCESS];
pthread_mutex_t mutex;
pthread_cond_t cv;
uint32_t current_process = 0;
uint32_t num_processes = 0;

/** Check if all processes are finished */
static bool is_all_process_finished() {
    for (uint32_t i = 0; i < num_processes; ++i) {
        if (!process[i].is_finished)
            return false;
    }
    return true;
}

/** Process execution function */
void* process_execution(void* arg) {
    Process* p = (Process*) arg;

    while (p->remaining_time > 0) {
        pthread_mutex_lock(&mutex);

        // Wait until it's this process's turn
        while (current_process != p->id - 1) {
            pthread_cond_wait(&cv, &mutex);
        }

        // Execute for time quantum or remaining time
        printf("Process %d starts executing\n", p->id);
        uint32_t exec_time = (p->remaining_time > MAX_TIME_QUANTUM) ? MAX_TIME_QUANTUM : p->remaining_time;
        sleep(exec_time);
        p->remaining_time -= exec_time;
        printf("Process %d time quantum over.\n", p->id);

        // Mark process as finished if no remaining time
        if (p->remaining_time == 0) {
            p->is_finished = true;
            printf("Process %d completed\n", p->id);
        }

        // Select next process in a strict round-robin fashion
        if (!is_all_process_finished()) {
            do {
                current_process = (current_process + 1) % num_processes;
            } while (process[current_process].is_finished);
        }

        // Broadcast to wake up the next process, but only one will proceed
        pthread_cond_broadcast(&cv);
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

int main() {
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cv, NULL);

    printf("Enter number of processes: ");
    scanf("%d", &num_processes);

    for (uint32_t i = 0; i < num_processes; ++i) {
        process[i].id = i + 1;
        printf("Enter burst time for process %d: ", process[i].id);
        scanf("%d", &process[i].burst_time);
        process[i].remaining_time = process[i].burst_time;
        process[i].is_finished = false;
    }

    // Create process threads
    for (uint32_t i = 0; i < num_processes; ++i) {
        pthread_create(&process[i].thread_id, NULL, process_execution, (void*) &process[i]);
    }

    // Wait for all processes to finish
    for (uint32_t i = 0; i < num_processes; ++i) {
        pthread_join(process[i].thread_id, NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cv);

    return 0;
}
