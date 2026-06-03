#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "rec_mutex.h"

/* Global recursive mutex for testing */
rec_mutex_t g_rec_mutex;

/* Test counters */
int test_count = 0;
int test_passed = 0;
int test_failed = 0;

/* Helper macro for tests */
#define TEST(name, condition) \
    do { \
        test_count++; \
        if (condition) { \
            printf("✓ Test %d PASSED: %s\n", test_count, name); \
            test_passed++; \
        } else { \
            printf("✗ Test %d FAILED: %s\n", test_count, name); \
            test_failed++; \
        } \
    } while(0)

/* ============== Test 1: Basic Initialization ============== */
void test_basic_init()
{
    printf("\n=== Test 1: Basic Initialization ===\n");
    
    rec_mutex_t mtx;
    int ret = rec_mutex_init(&mtx);
    
    TEST("Initialize recursive mutex", ret == 0);
    TEST("Destroy recursive mutex", rec_mutex_destroy(&mtx) == 0);
}

/* ============== Test 2: Basic Lock/Unlock ============== */
void test_basic_lock_unlock()
{
    printf("\n=== Test 2: Basic Lock/Unlock ===\n");
    
    rec_mutex_t mtx;
    rec_mutex_init(&mtx);
    
    int ret1 = rec_mutex_lock(&mtx);
    TEST("Lock acquired successfully", ret1 == 0);
    
    int ret2 = rec_mutex_unlock(&mtx);
    TEST("Unlock successful", ret2 == 0);
    
    rec_mutex_destroy(&mtx);
}

/* ============== Test 3: Recursive Locking ============== */
void test_recursive_locking()
{
    printf("\n=== Test 3: Recursive Locking (Same Thread) ===\n");
    
    rec_mutex_t mtx;
    rec_mutex_init(&mtx);
    
    /* Lock multiple times from same thread */
    int ret1 = rec_mutex_lock(&mtx);
    TEST("First lock acquired", ret1 == 0);
    
    int ret2 = rec_mutex_lock(&mtx);
    TEST("Second lock acquired (recursive)", ret2 == 0);
    
    int ret3 = rec_mutex_lock(&mtx);
    TEST("Third lock acquired (recursive)", ret3 == 0);
    
    /* Unlock in reverse order */
    int ret4 = rec_mutex_unlock(&mtx);
    TEST("First unlock successful", ret4 == 0);
    
    int ret5 = rec_mutex_unlock(&mtx);
    TEST("Second unlock successful", ret5 == 0);
    
    int ret6 = rec_mutex_unlock(&mtx);
    TEST("Third unlock successful", ret6 == 0);
    
    rec_mutex_destroy(&mtx);
}

/* ============== Test 4: Error Handling - Unlock without Lock ============== */
void test_unlock_without_lock()
{
    printf("\n=== Test 4: Error Handling - Unlock without Lock ===\n");
    
    rec_mutex_t mtx;
    rec_mutex_init(&mtx);
    
    /* Try to unlock without lock */
    int ret = rec_mutex_unlock(&mtx);
    TEST("Unlock without lock returns error", ret == -1);
    
    rec_mutex_destroy(&mtx);
}

/* ============== Test 5: NULL Pointer Handling ============== */
void test_null_pointer()
{
    printf("\n=== Test 5: NULL Pointer Handling ===\n");
    
    TEST("Init with NULL pointer returns error", rec_mutex_init(NULL) == -1);
    TEST("Lock with NULL pointer returns error", rec_mutex_lock(NULL) == -1);
    TEST("Unlock with NULL pointer returns error", rec_mutex_unlock(NULL) == -1);
    TEST("Destroy with NULL pointer returns error", rec_mutex_destroy(NULL) == -1);
}

/* ============== Test 6: Multi-threaded Access ============== */
typedef struct {
    int thread_id;
    int iterations;
    int success_count;
} thread_args_t;

void* thread_worker(void* arg)
{
    thread_args_t* args = (thread_args_t*)arg;
    args->success_count = 0;
    
    for (int i = 0; i < args->iterations; i++) {
        if (rec_mutex_lock(&g_rec_mutex) == 0) {
            /* Critical section */
            usleep(100);  /* Simulate some work */
            
            if (rec_mutex_unlock(&g_rec_mutex) == 0) {
                args->success_count++;
            }
        }
    }
    
    return arg;
}

void test_multi_threaded()
{
    printf("\n=== Test 6: Multi-threaded Access ===\n");
    
    rec_mutex_init(&g_rec_mutex);
    
    pthread_t threads[3];
    thread_args_t args[3];
    
    /* Create 3 threads */
    for (int i = 0; i < 3; i++) {
        args[i].thread_id = i;
        args[i].iterations = 5;
        args[i].success_count = 0;
        pthread_create(&threads[i], NULL, thread_worker, &args[i]);
    }
    
    /* Wait for all threads */
    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
        printf("Thread %d: %d/%d successful operations\n", 
               i, args[i].success_count, args[i].iterations);
        TEST("Thread completed all operations", args[i].success_count == args[i].iterations);
    }
    
    rec_mutex_destroy(&g_rec_mutex);
}

/* ============== Test 7: Recursive Lock in Nested Function ============== */
typedef struct {
    int value;
} shared_resource_t;

shared_resource_t g_resource = {0};

int increment_resource()
{
    /* First lock (from caller) is already held */
    if (rec_mutex_lock(&g_rec_mutex) != 0)
        return -1;
    
    g_resource.value++;
    
    if (rec_mutex_unlock(&g_rec_mutex) != 0)
        return -1;
    
    return g_resource.value;
}

void test_nested_locking()
{
    printf("\n=== Test 7: Nested Recursive Locking ===\n");
    
    rec_mutex_init(&g_rec_mutex);
    g_resource.value = 0;
    
    /* Outer lock */
    rec_mutex_lock(&g_rec_mutex);
    
    /* Call function that tries to lock again (should succeed) */
    int result1 = increment_resource();
    TEST("Nested lock/unlock in function succeeded", result1 == 1);
    
    int result2 = increment_resource();
    TEST("Second nested lock/unlock in function succeeded", result2 == 2);
    
    /* Outer unlock */
    rec_mutex_unlock(&g_rec_mutex);
    
    TEST("Resource value is correct", g_resource.value == 2);
    
    rec_mutex_destroy(&g_rec_mutex);
}

/* ============== Test 8: Wrong Thread Unlock ============== */
typedef struct {
    pthread_t locker_thread;
    int lock_attempted;
} lock_state_t;

lock_state_t g_lock_state = {0, 0};

void* lock_holding_thread(void* arg)
{
    (void)arg;  /* Unused parameter */
    
    rec_mutex_lock(&g_rec_mutex);
    g_lock_state.locker_thread = pthread_self();
    g_lock_state.lock_attempted = 1;
    
    sleep(1);  /* Hold the lock for 1 second */
    
    rec_mutex_unlock(&g_rec_mutex);
    return NULL;
}

void test_wrong_thread_unlock()
{
    printf("\n=== Test 8: Wrong Thread Unlock (Error Case) ===\n");
    
    rec_mutex_init(&g_rec_mutex);
    
    pthread_t thread;
    pthread_create(&thread, NULL, lock_holding_thread, NULL);
    
    /* Wait for other thread to acquire lock */
    usleep(100000);  /* 100ms */
    
    /* Try to unlock from different thread (should fail) */
    int ret = rec_mutex_unlock(&g_rec_mutex);
    TEST("Wrong thread cannot unlock mutex", ret == -1);
    
    pthread_join(thread, NULL);
    rec_mutex_destroy(&g_rec_mutex);
}

/* ============== Main Test Runner ============== */
int main()
{
    printf("╔════════════════════════════════════════════════════════╗\n");
    printf("║         Recursive Mutex Test Suite                    ║\n");
    printf("╚════════════════════════════════════════════════════════╝\n");
    
    test_basic_init();
    test_basic_lock_unlock();
    test_recursive_locking();
    test_unlock_without_lock();
    test_null_pointer();
    test_multi_threaded();
    test_nested_locking();
    test_wrong_thread_unlock();
    
    printf("\n╔════════════════════════════════════════════════════════╗\n");
    printf("║                   TEST SUMMARY                        ║\n");
    printf("╠════════════════════════════════════════════════════════╣\n");
    printf("║ Total Tests:  %d\n", test_count);
    printf("║ Passed:       %d ✓\n", test_passed);
    printf("║ Failed:       %d ✗\n", test_failed);
    printf("╚════════════════════════════════════════════════════════╝\n");
    
    if (test_failed == 0) {
        printf("\n🎉 All tests passed!\n");
        return 0;
    } else {
        printf("\n❌ Some tests failed!\n");
        return 1;
    }
}
