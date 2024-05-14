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
    if (queue_out(&mutex_queue, &mutex_id) == -1 || queue_is_empty(&mutex_queue)) {
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
    mutex->locks = 0;
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
    mutex_t *mutex = &mutexes[id];
    // look up the mutex in the mutex table
    if (id < 0 || id >= MUTEX_MAX) {
        return -1;
    }

    // If the mutex is locked, prevent it from being destroyed (return error)
    if (mutex->locks > 0) {
        return -1; // Error: the mutex is locked
    }

    // Add the id back into the mutex queue to be re-used later
    queue_in(&mutex_queue, id);

    // Clear the memory for the data structure
    memset(mutex, 0, sizeof(mutex_t));

    return 0;
}

/**
 * Locks the specified mutex
 * @param id - the mutex id
 * @return -1 on error, otherwise the current lock count
 */
int kmutex_lock(int id) {
    mutex_t *mutex = &mutexes[id];
    // look up the mutex in the mutex table
    if (id < 0 || id >= MUTEX_MAX) {
        return -1;
    }
    proc_t *active_process = active_proc;
    // If the mutex is already locked
    //   1. Set the active process state to WAITING
    //   2. Add the process to the mutex wait queue (so it can take
    //      the mutex when it is unlocked)
    //   3. Remove the process from the scheduler, allow another
    //      process to be scheduled

    // If the mutex is not locked
    //   1. set the mutex owner to the active process
    
    if (mutex->locks > 0) {
        // Add the process to the mutex wait queue
        if (active_process) {
            queue_in(&mutex->wait_queue, active_process->pid);
            // Set the state of the active process to WAITING
            active_process->state = WAITING;
            // Remove the process from the scheduler
            scheduler_remove(active_process);
        }
    } else {
        // Set the mutex owner to the current process
        mutex->owner = active_process;
    }

    // Increment the lock count
    mutex->locks++;

    // Return the mutex lock count
    return mutex->locks;
}

/**
 * Unlocks the specified mutex
 * @param id - the mutex id
 * @return -1 on error, otherwise the current lock count
 */
int kmutex_unlock(int id) {
    mutex_t *mutex = &mutexes[id];
    // look up the mutex in the mutex table
    if (id < 0 || id >= MUTEX_MAX) {
        return -1;
    }
    proc_t *active_process = active_proc;

    // If the mutex is not locked, there is nothing to do
    if (mutex->locks == 0) {
        return 0;
    }
    // If the mutex is held but not owned by the calling process, return error
    if (mutex->owner != active_process) {
        return -1; // Error: the mutex is held but not owned by the calling process
    }

    // Decrement the lock count
    mutex->locks--;

    // If there are no more locks held:
    //    1. clear the owner of the mutex

    // If there are still locks held:
    //    1. Obtain a process from the mutex wait queue
    //    2. Add the process back to the scheduler
    //    3. set the owner of the of the mutex to the process

    if (mutex->locks == 0) {
        mutex->owner = NULL;
    } else {
        /*int waiting_process = queue_out(&mutex->wait_queue, &waiting_process);
        mutex->owner = waiting_process;
        scheduler_add(waiting_process);*/
        int waiting_pid;
        if (queue_out(&mutex->wait_queue, &waiting_pid) == 0) {
            proc_t *waiting_proc = pid_to_proc(waiting_pid);
            if (waiting_proc) {
                waiting_proc->state = READY;
                scheduler_add(waiting_proc);
                mutex->owner = waiting_proc;
            }
        }
    }
    // return the mutex lock count
    return mutex->locks;
}
