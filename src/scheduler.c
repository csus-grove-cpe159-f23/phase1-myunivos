/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 *
 * Kernel Process Handling
 */

#include <spede/string.h>
#include <spede/stdio.h>
#include <spede/time.h>
#include <spede/machine/proc_reg.h>

#include "kernel.h"
#include "kproc.h"
#include "scheduler.h"
#include "timer.h"

#include "queue.h"

// Process Queues
queue_t run_queue;

/**
 * Scheduler timer callback
 */
void scheduler_timer(void) {
    // Update the active process' run time and CPU time
    if (active_proc != NULL) {
        active_proc->cpu_time++;

        if (active_proc->cpu_time >= TIME_SLICE) {
            // Time slice expired, add to the run queue if not the idle process
            if (active_proc->pid != 0) {
                queue_in(&run_queue, active_proc->pid);
            }
            active_proc->cpu_time = 0;
            active_proc->state = IDLE;
            active_proc = NULL; // No active process now
        }
    }

}

/**
 * Executes the scheduler
 * Should ensure that `active_proc` is set to a valid process entry
 */
void scheduler_run(void) {
    // Ensure that processes not in the active state aren't still scheduled

    // Check if we have an active process
        // Check if the current process has exceeded it's time slice
            // Reset the active time

            // If the process is not the idle task, add it back to the scheduler
            // Otherwise, simply set the state to IDLE

            // Unschedule the active process

    // Check if we have a process scheduled or not
        // Get the proces id from the run queue
            // default to process id 0 (idle task) if a process can't be scheduled

        // Update the active proc pointer

    // Make sure we have a valid process at this point

    // Ensure that the process state is set
    if (active_proc == NULL || active_proc->state != ACTIVE) {
        int pid;
        if (!queue_is_empty(&run_queue)) {
            // If the run queue is not empty, get the next process
            queue_out(&run_queue, &pid);
            active_proc = pid_to_proc(pid); // Convert PID to process control block
        } else {
            // No process is ready to run, assign idle process
            active_proc = pid_to_proc(0); // The idle task has a PID of 0
        }

        if (active_proc != NULL) {
            active_proc->state = ACTIVE;
            // Additional context switch code would go here
        }
    }
}

/**
 * Adds a process to the scheduler
 * @param proc - pointer to the process entry
 */
void scheduler_add(proc_t *proc) {
    // Add the process to the run queue
    // Set the process state
    if (proc->pid != 0 && proc->state != ACTIVE) {
        queue_in(&run_queue, proc->pid);
        proc->state = IDLE; // Mark as idle until it is scheduled again
    }
}

/**
 * Removes a process from the scheduler
 * @param proc - pointer to the process entry
 */
void scheduler_remove(proc_t *proc) {
    // Iterate through each the process queue
    // If the processis found, skip it; otherwise, ensure that each other process remains in the queue

    // If the process is the active process, ensure that the active process is cleared so when the
    // scheduler runs again, it will select a new process to run
    if (active_proc == proc) {
        active_proc = NULL; // Unschedule the active process
    }
    proc->state = NONE; // Set state to NONE indicating it's not in the scheduler
}

/**
 * Initializes the scheduler, data structures, etc.
 */
void scheduler_init(void) {
    kernel_log_info("Initializing scheduler");

    // Initialize any data structures or variables
    // Register the timer callback (scheduler_timer) to run every tick
    queue_init(&run_queue); // Initialize the run queue
    timer_callback_register(scheduler_timer, 1, -1); // Register scheduler timer to tick every time
}

