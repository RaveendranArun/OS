# Recursive Mutex Test Suite

## Overview

`rec_mutex_test.c` provides comprehensive testing for the recursive mutex implementation. It covers functionality, error handling, and multi-threaded scenarios.

## Test Cases

### Test 1: Basic Initialization
- Tests mutex initialization and cleanup
- Verifies successful init/destroy operations

### Test 2: Basic Lock/Unlock
- Tests simple lock and unlock operations
- Verifies single lock/unlock cycle

### Test 3: Recursive Locking
- Tests the core recursive feature: same thread acquiring lock multiple times
- Demonstrates nested lock/unlock semantics
- Verifies all locks can be acquired and released successfully

### Test 4: Error Handling - Unlock without Lock
- Tests error case: attempting to unlock without acquiring lock
- Verifies -1 return code for error condition

### Test 5: NULL Pointer Handling
- Tests robustness with NULL pointer arguments
- Verifies all functions handle NULL gracefully

### Test 6: Multi-threaded Access
- Creates 3 threads competing for the same recursive mutex
- Each thread performs 5 lock/unlock cycles
- Verifies mutual exclusion between threads
- Tests serialized access to critical section

### Test 7: Nested Recursive Locking
- Demonstrates practical use case: nested function calls
- Outer function locks the mutex
- Inner function can also lock/unlock (recursively)
- Shows real-world reentrancy pattern

### Test 8: Wrong Thread Unlock
- Tests error case: thread attempting to unlock mutex owned by another thread
- Verifies -1 return code (mutex ownership checking)

## Compilation

### Using Makefile (Recommended)
```bash
make              # Build the test executable
make run          # Build and run tests
make test         # Same as 'make run'
make clean        # Remove build artifacts
```

### Manual Compilation
```bash
gcc -Wall -Wextra -pthread -std=c99 -O2 -o rec_mutex_test rec_mutex.c rec_mutex_test.c
```

## Running Tests

```bash
# Run all tests
./rec_mutex_test

# Or using make
make test
```

## Expected Output

```
╔════════════════════════════════════════════════════════╗
║         Recursive Mutex Test Suite                    ║
╚════════════════════════════════════════════════════════╝

=== Test 1: Basic Initialization ===
✓ Test 1 PASSED: Initialize recursive mutex
✓ Test 2 PASSED: Destroy recursive mutex

=== Test 2: Basic Lock/Unlock ===
✓ Test 3 PASSED: Lock acquired successfully
✓ Test 4 PASSED: Unlock successful

=== Test 3: Recursive Locking (Same Thread) ===
✓ Test 5 PASSED: First lock acquired
✓ Test 6 PASSED: Second lock acquired (recursive)
✓ Test 7 PASSED: Third lock acquired (recursive)
✓ Test 8 PASSED: First unlock successful
✓ Test 9 PASSED: Second unlock successful
✓ Test 10 PASSED: Third unlock successful

=== Test 4: Error Handling - Unlock without Lock ===
✓ Test 11 PASSED: Unlock without lock returns error

=== Test 5: NULL Pointer Handling ===
✓ Test 12 PASSED: Init with NULL pointer returns error
✓ Test 13 PASSED: Lock with NULL pointer returns error
✓ Test 14 PASSED: Unlock with NULL pointer returns error
✓ Test 15 PASSED: Destroy with NULL pointer returns error

=== Test 6: Multi-threaded Access ===
Thread 0: 5/5 successful operations
✓ Test 16 PASSED: Thread completed all operations
Thread 1: 5/5 successful operations
✓ Test 17 PASSED: Thread completed all operations
Thread 2: 5/5 successful operations
✓ Test 18 PASSED: Thread completed all operations

=== Test 7: Nested Recursive Locking ===
✓ Test 19 PASSED: Nested lock/unlock in function succeeded
✓ Test 20 PASSED: Second nested lock/unlock in function succeeded
✓ Test 21 PASSED: Resource value is correct

=== Test 8: Wrong Thread Unlock (Error Case) ===
✓ Test 22 PASSED: Wrong thread cannot unlock mutex

╔════════════════════════════════════════════════════════╗
║                   TEST SUMMARY                        ║
╠════════════════════════════════════════════════════════╣
║ Total Tests:  22
║ Passed:       22 ✓
║ Failed:       0 ✗
╚════════════════════════════════════════════════════════╝

🎉 All tests passed!
```

## Key Features Tested

✅ **Reentrancy**: Same thread can lock multiple times  
✅ **Mutual Exclusion**: Different threads block each other  
✅ **Error Handling**: Invalid operations return -1  
✅ **Thread Safety**: Condition variables coordinate waiting threads  
✅ **Ownership**: Only lock owner can unlock  
✅ **Resource Cleanup**: Proper init/destroy lifecycle  

## Test Design Patterns

1. **Unit Tests**: Individual function tests with isolation
2. **Integration Tests**: Multi-threaded scenarios
3. **Error Tests**: Edge cases and invalid operations
4. **Real-World Scenarios**: Practical usage patterns

## Notes

- Tests use `pthread_create()` and `pthread_join()` for multi-threading
- Some tests use `sleep()` and `usleep()` for synchronization
- All tests verify both success and failure cases
- Test results are color-coded (✓ passed, ✗ failed)
