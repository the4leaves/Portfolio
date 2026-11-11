POSIX threading API
===================

You do not need to use all of these synchronization constructs. You may use any subset of them which allows you to complete the project.

## Thread management

The test driver program for this project will automatically manage your application threads, but you can reference the documentation page for the following functions if you want to change it to help you test your program:

- [`pthread_create`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_create.html)
- [`pthread_join`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_join.html)

A new thread is created using `pthread_create()`, which returns a handle to the created thread, and its result is waited for via a call to `pthread_join()`.

## Mutexes (Locks)

- [`pthread_mutex_init`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_mutex_init.html)
- [`pthread_mutex_destroy`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_mutex_init.html)
- [`pthread_mutex_lock`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_mutex_lock.html)
- [`pthread_mutex_unlock`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_mutex_lock.html)

To declare and initialize a mutex, you may use the following code. (You do not need to worry about mutex attributes for this project.)

```c
pthread_mutex_t mutex;

// ...

pthread_mutex_init(&mutex, NULL);
```

To destroy a unlocked mutex during cleanup, use `pthread_mutex_destroy`:
```c
pthread_mutex_destroy(&mutex);
```

Locking and unlocking the mutex should be straightforward. To make it easier to recognize code in locked sections, we recommend indenting critical sections protected by a mutex as follows:
```c
pthread_mutex_lock(&mutex);
{
	// Critical region protected by mutex
}
pthread_mutex_unlock(&mutex);
```
## Semaphores

- [`int sem_init(sem_t *sem, int pshared, unsigned int value);`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/sem_init.html)
- [`int sem_post(sem_t *sem);`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/sem_post.html)
- [`int sem_wait(sem_t *sem);`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/sem_wait.html)
- [`int sem_destroy(sem_t *sem);`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/sem_destroy.html)
- [`int sem_getvalue(sem_t *sem, int *valp);`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/sem_getvalue.html)
 
All POSIX semaphore functions and types are prototyped or defined in `semaphore.h`. To define a semaphore object, use
```c
 sem_t sem_name;
```

To initialize a semaphore, use `sem_init`
```c
sem_init(&sem_name, 0, 10);
```
In this example, the value of the semaphore is 10;

Use `sem_wait` to wait on a semaphore. If the value is negative, the calling process blocks. The waiting process wakes up when another process calls `sem_post`.
```c
sem_wait(&sem_name);
```

Use `sem_post` to increment the value and wake up any blocked process.
```c
sem_post(&sem_name);
```

To get the current value of the semaphore, use `sem_getvalue`
```c
int value; 
sem_getvalue(&sem_name, &value); 
printf("Semaphore value is %d\n", value);
```

To destroy a semaphore, use `sem_destroy`
```c
sem_destroy(&sem_name); 
```
Destruction succeeds only when no threads is waiting on the semaphore. 
<!--- 
## Condition variables

- [`pthread_cond_init`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_cond_init.html)
- [`pthread_cond_destroy`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_cond_init.html)
- [`pthread_cond_wait`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_cond_wait.html)
- [`pthread_cond_signal`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_cond_signal.html)
- [`pthread_cond_broadcast`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_cond_broadcast.html)

Initialization of a condition variable follows the same form as initialization of a mutex.

As discussed in lecture, to wait on a condition variable, you must first acquire the mutex associated with the resource, and then repeatedly test the predicate until the desired condition has been achieved (due to the potential of spurious wakeups):

```c
pthread_mutex_lock(&mutex);
{
	while (!condition) {
		pthread_cond_wait(&condvar, &mutex);
	}

	// Condition is now true and the mutex is relocked
}
pthread_mutex_unlock(&mutex);
```

To wake up one thread waiting on a condition variable, use `pthread_cond_signal`:
```c
pthread_cond_signal(&condvar);
```

To wake up all threads waiting on a condition variable, use `pthread_cond_broadcast`:
```c
pthread_cond_broadcast(&condvar);
```

## Readers-writers locks

- [`pthread_rwlock_init`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_rwlock_init.html)
- [`pthread_rwlock_destroy`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_rwlock_init.html)
- [`pthread_rwlock_rdlock`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_rwlock_rdlock.html)
- [`pthread_rwlock_wrlock`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_rwlock_wrlock.html)
- [`pthread_rwlock_unlock`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_rwlock_unlock.html)

Initialization of a readers-writers lock follows the same form as initialization of a mutex. As before, we recommend you indent code in locked sections to make it easier to recognize.
```c
pthread_rwlock_rdlock(&rwlock);
{
	// Resource locked for reading. Other reader threads may enter.
}
pthread_rwlock_unlock(&rwlock);

pthread_rwlock_wrlock(&rwlock);
{
	// Resource locked for writing. This is the only thread.
}
pthread_rwlock_unlock(&rwlock);
```

## Barriers

Note: barriers are not available on macOS, but you are free to use them in your project, as the project will be compiled and graded on Linux.

- [`pthread_barrier_init`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_barrier_init.html)
- [`pthread_barrier_destroy`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_barrier_init.html)
- [`pthread_barrier_wait`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_barrier_wait.html)

To declare and initialize a barrier, you may use the following code, where `number_of_threads` refers to the number of threads that need to wait on the barrier before execution can continue.

```c
pthread_barrier_t barrier;

// ...

pthread_barrier_init(&barrier, NULL, number_of_threads);
```

To synchronize threads on a barrier, use `pthread_barrier_wait`:
```c
pthread_barrier_wait(&barrier);
```
-->
