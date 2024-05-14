/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 *
 * Kernel Semaphores
 */

#include <spede/string.h>

#include "kernel.h"
#include "ksem.h"
#include "queue.h"
#include "scheduler.h"
#include "kproc.h"

// Table of all semephores
sem_t semaphores[SEM_MAX];

// semaphore ids to be allocated
queue_t sem_queue;

/**
 * Initializes kernel semaphore data structures
 * @return -1 on error, 0 on success
 */
int ksemaphores_init() {
    kernel_log_info("Initializing kernel semaphores");

    // Initialize the semaphore table

    // Initialize the semaphore queue

    // Fill the semaphore queue
    int i;

    queue_init(&sem_queue);
    for (i = 0; i < SEM_MAX; i++) {
        semaphores[i].allocated = 0;
        semaphores[i].count = 0;
        queue_init(&semaphores[i].wait_queue);
        queue_in(&sem_queue, i);
    }
    return 0;
}

/**
 * Allocates a semaphore
 * @param value - initial semaphore value
 * @return -1 on error, otherwise the semaphore id that was allocated
 */
int ksem_init(int value) {
    // Obtain a semaphore id from the semaphore queue

    // Ensure that the id is within the valid range

    // Initialize the semaphore data structure
    // sempohare table + all members (wait queue, allocated, count)
        // set count to initial value
    int id;
    if(queue_out(&sem_queue, &id) == -1 || id < 0 || id >= SEM_MAX) {
    return -1; //No available semaphore  or Invalid Id
    }

    semaphores[id].allocated = 1;
    semaphores[id].count = value;
    queue_init(&semaphores[id].wait_queue);
    return id;
}

/**
 * Frees the specified semaphore
 * @param id - the semaphore id
 * @return 0 on success, -1 on error
 */
int ksem_destroy(int id) {
    // look up the sempaphore in the semaphore table

    // If the semaphore is locked, prevent it from being destroyed

    // Add the id back into the semaphore queue to be re-used later

    // Clear the memory for the data structure
    if(id < 0 || id >= SEM_MAX || semaphores[id].allocated == 0) {
        return -1;
    }
    if(!queue_is_empty(&semaphores[id].wait_queue)){
        return -1; //cannot destroy semaphore, process is waiting
    }

    semaphores[id].allocated = 0;
    queue_in(&sem_queue, id);

    return 0;
}

/**
 * Waits on the specified semaphore if it is held
 * @param id - the semaphore id
 * @return -1 on error, otherwise the current semaphore count
 */
int ksem_wait(int id) {
    // look up the sempaphore in the semaphore table

    // If the semaphore count is 0, then the process must wait
        // Set the state to WAITING
        // add to the semaphore's wait queue
        // remove from the scheduler

    // If the semaphore count is > 0
        // Decrement the count

    // Return the current semaphore count

    if (id < 0 || id >= SEM_MAX || semaphores[id].allocated == 0) {
        return -1;
    }

    if (semaphores[id].count > 0) {
        semaphores[id].count--;
        return semaphores[id].count;
    }

    // Block the process
    scheduler_remove(active_proc);
    active_proc->state = WAITING;
    queue_in(&semaphores[id].wait_queue, active_proc->pid);
    return 0;
}

/**
 * Posts the specified semaphore
 * @param id - the semaphore id
 * @return -1 on error, otherwise the current semaphore count
 */
int ksem_post(int id) {

    // look up the semaphore in the semaphore table

    // incrememnt the semaphore count

    // check if any processes are waiting on the semaphore (semaphore wait queue)
        // if so, queue out and add to the scheduler
        // decrement the semaphore count

    // return current semaphore count
    if (id < 0 || id >= SEM_MAX || semaphores[id].allocated == 0) {
        return -1;
    }

    semaphores[id].count++;
    if (!queue_is_empty(&semaphores[id].wait_queue)) {
        int pid;
        if (queue_out(&semaphores[id].wait_queue, &pid) != 0) {
            // Error handling if queue_out fails
            return -1;
        }
        // Check if pid is a valid process ID
        if (pid >= 0 && pid < PROC_MAX && proc_table[pid].state != NONE) {
            proc_t *proc = &proc_table[pid];
            scheduler_add(proc);
            semaphores[id].count--;
        } else {
            // Error handling if pid is not valid or the corresponding process doesn't exist
            return -1;
        }
        /*queue_out(&semaphores[id].wait_queue, &pid);
        proc_t *proc = &proc_table[pid];
        scheduler_add(proc);
        semaphores[id].count--;*/
    }

    return semaphores[id].count;
}
