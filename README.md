A minimal implementation of posix semaphore. A synchronization method which could be used for either process or thread synchronization.
At present, this implemenation supports only thread synchronization. In the sema_init(), the second argument is negleted.
The building blocks of this implementation are posix mutex and condvar mechanism
The implementation is tested with an illustrsation of strict alternation between threads.
