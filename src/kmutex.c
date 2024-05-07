/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 *
 * Kernel Mutexes
 */

#include <spede/string.h>

#include "kernel.h"
#include "kmutex.h"
#include "queue.h"
#include "scheduler.h"

// Table of all mutexes
mutex_t mutexes[MUTEX_MAX];

// Mutex ids to be allocated
queue_t mutex_queue;

/**
 * Initializes kernel mutex data structures
 * @return -1 on error, 0 on success
 */
int kmutexes_init() {
    kernel_log_info("Initializing kernel mutexes");

    // Initialize the mutex table
    memset(mutexes, 0, sizeof(mutexes));

    // Initialize the mutex queue
    queue_init(&mutex_queue);

    // Fill the mutex queue
    for (int i = 0; i < MUTEX_MAX; i++) {
        queue_in(&mutex_queue, i);
    }

    return 0;
}

/**
 * Allocates a mutex
 * @return -1 on error, otherwise the mutex id that was allocated
 */
int kmutex_init(void) {
    // Obtain a mutex id from the mutex queue
    int mutex_id;
    if (queue_out(&mutex_queue, &mutex_id) == queue_is_empty(&mutex_queue) {
        return -1;
    }

    // Ensure that the id is within the valid range
    if (mutex_id < 0 || mutex_id >= MUTEX_MAX) {
        return -1;
    }

    // Pointer to the mutex table entry
    mutex_t *mutex = &mutexes[mutex_id];

    // Initialize the mutex data structure (mutex_t + all members)
    memset(mutex, 0, sizeof(mutex_t));
    queue_init(&mutex->wait_queue);
    mutex->allocated = 1;
    mutex->lock_count = 0;
    mutex->owner = NULL;

    // return the mutex id

    return mutex_id;
}

/**
 * Frees the specified mutex
 * @param id - the mutex id
 * @return 0 on success, -1 on error
 */
int kmutex_destroy(int id) {
    // look up the mutex in the mutex table

    // If the mutex is locked, prevent it from being destroyed (return error)

    // Add the id back into the mutex queue to be re-used later

    // Clear the memory for the data structure

    return -1;
}

/**
 * Locks the specified mutex
 * @param id - the mutex id
 * @return -1 on error, otherwise the current lock count
 */
int kmutex_lock(int id) {
    // look up the mutex in the mutex table

    // If the mutex is already locked
    //   1. Set the active process state to WAITING
    //   2. Add the process to the mutex wait queue (so it can take
    //      the mutex when it is unlocked)
    //   3. Remove the process from the scheduler, allow another
    //      process to be scheduled

    // If the mutex is not locked
    //   1. set the mutex owner to the active process

    // Increment the lock count

    // Return the mutex lock count

    return -1;
}

/**
 * Unlocks the specified mutex
 * @param id - the mutex id
 * @return -1 on error, otherwise the current lock count
 */
int kmutex_unlock(int id) {
    // look up the mutex in the mutex table

    // If the mutex is not locked, there is nothing to do

    // Decrement the lock count

    // If there are no more locks held:
    //    1. clear the owner of the mutex

    // If there are still locks held:
    //    1. Obtain a process from the mutex wait queue
    //    2. Add the process back to the scheduler
    //    3. set the owner of the of the mutex to the process

    // return the mutex lock count

    return -1;
}
