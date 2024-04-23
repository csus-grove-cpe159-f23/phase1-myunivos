/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 *
 * Kernel System Call Handlers
 */
#include <spede/time.h>
#include <spede/string.h>
#include <spede/stdio.h>

#include "kernel.h"
#include "kproc.h"
#include "ksyscall.h"
#include "interrupts.h"
#include "scheduler.h"
#include "timer.h"

#define ACC_INTR_GATE 0x8E00
extern void fill_gate(int vector, int addr, int cs, int attr, int dpl);
extern unsigned short get_cs(void);
extern queue_t sleep_queue; 
/**
 * System call IRQ handler
 * Dispatches system calls to the function associate with the specified system call
 */
void ksyscall_irq_handler(void) {
    // Default return value
    int rc = -1;

    if (!active_proc) {
        kernel_panic("Invalid process");
    }

    if (!active_proc->trapframe) {
        kernel_panic("Invalid trapframe");
    }

    // System call identifier is stored on the EAX register
    // Additional arguments should be stored on additional registers (EBX, ECX, etc.)

    // Based upon the system call identifier, call the respective system call handler

    // Ensure that the EAX register for the active process contains the return value

    if (active_proc->trapframe->eax == SYSCALL_SYS_GET_TIME) {
        rc = ksyscall_sys_get_time();
        active_proc->trapframe->eax = rc;
        return;
    }

    if (active_proc->trapframe->eax == SYSCALL_SYS_GET_NAME) {
        // Cast the argument as a char pointer
        rc = ksyscall_sys_get_name((char *)active_proc->trapframe->ebx);
        active_proc->trapframe->eax = rc;
        return;
    }

    kernel_panic("Invalid system call %d!", active_proc->trapframe->eax);
}

/**
 * System Call Initialization
 */
void ksyscall_init(void) {
    // Register the IDT entry and IRQ handler for the syscall IRQ (IRQ_SYSCALL)
    fill_gate(IRQ_SYSCALL, (unsigned int)ksyscall_irq_handler, get_cs(), ACC_INTR_GATE, 0);
}

/**
 * Writes up to n bytes to the process' specified IO buffer
 * @param io - the IO buffer to write to
 * @param buf - the buffer to copy from
 * @param n - number of bytes to write
 * @return -1 on error or value indicating number of bytes copied
 */
int ksyscall_io_write(int io, char *buf, int size) {

    // Ensure there is an active process

    // Ensure the IO buffer is withing range (PROC_IO_MAX)

    // Ensure that the active process has valid io
    // If not active_proc->....

    // Using ringbuf_write_mem - Write size bytes from buf to active_proc->io... 
    if (!active_proc) {
        return -1; // No active process
    }

    // Validate IO index and that the active process has a valid io buffer.
    if (io < 0 || io >= PROC_IO_MAX || !active_proc->io[io]) {
        return -1; // Invalid IO index or buffer
    }

     return ringbuf_write_mem(active_proc->io[io], buf, size);
}

/**
 * Reads up to n bytes from the process' specified IO buffer
 * @param io - the IO buffer to read from
 * @param buf - the buffer to copy to
 * @param n - number of bytes to read
 * @return -1 on error or value indicating number of bytes copied
 */
int ksyscall_io_read(int io, char *buf, int size) {

    // Ensure there is active process, IO buffer is within range, active process has valid io

    // Using ringbuf_read_mem - Read size bytes from active_proc->io to buf
    if (!active_proc) {
        return -1; // No active process
    }

    // Check if IO index is within range and buffer is valid
    if (io < 0 || io >= PROC_IO_MAX || !active_proc->io[io]) {
        return -1; // Invalid IO index or buffer
    }

    // Read data from the ring buffer
    int bytes_read = ringbuf_read_mem(active_proc->io[io], buf, size);
    if (bytes_read < 0) {
        return -1; // Error during read
    }

    return bytes_read; // Return number of bytes read
}


/**
 * Flushes (clears) the specified IO buffer
 * @param io - the IO buffer to flush
 * @return -1 on error or 0 on success
 */
int ksyscall_io_flush(int io) {

    // Ensure active process, etc... 

    // Use ringbuf_flush to flush io buffer
    if (!active_proc) {
        return -1; // No active process
    }

    // Check if IO index is within range and buffer is valid
    if (io < 0 || io >= PROC_IO_MAX || !active_proc->io[io]) {
        return -1; // Invalid IO index or buffer
    }

    // Flush the ring buffer
    ringbuf_flush(active_proc->io[io]);

    return 0; // Success
}

/**
 * Gets the current system time (in seconds)
 * @return system time in seconds
 */
int ksyscall_sys_get_time(void) {
    return timer_get_ticks() / 100;
}

/**
 * Gets the operating system name
 * @param name - pointer to a character buffer where the name will be copied
 * @return 0 on success, -1 or other non-zero value on error
 */
int ksyscall_sys_get_name(char *name) {
    if (!name) {
        return -1;
    }

    strncpy(name, OS_NAME, sizeof(OS_NAME)-1);
    name[sizeof(OS_NAME)-1] = '\0';
    return 0;
}

/**
 * Puts the active process to sleep for the specified number of seconds
 * @param seconds - number of seconds the process should sleep
 */
int ksyscall_proc_sleep(int seconds) {
    if (!active_proc || seconds < 0) {
        return -1; // Invalid process or seconds
    }

    // Convert seconds to ticks assuming a certain number of ticks per second
    int ticks_per_second = 100; // This may differ for your setup
    active_proc->sleep_time = timer_get_ticks() + (seconds * ticks_per_second);
    active_proc->state = SLEEPING;

    // Remove process from active queue, and add to sleep queue
    scheduler_remove(active_proc);
    queue_in(&sleep_queue, active_proc->pid);

    return 0; // Success
}

/**
 * Exits the current process
 */
int ksyscall_proc_exit(void) {
    if (!active_proc) {
        return -1; // No active process
    }
    //kproc_destroy(active_proc);
    //scheduler_run_next();
    return 0;
}

/**
 * Gets the active process pid
 * @return process id or -1 on error
 */
int ksyscall_proc_get_pid(void) {
    if (!active_proc){
    return -1;
    }
    return active_proc->pid;
}

/**
 * Gets the active process' name
 * @param name - pointer to a character buffer where the name will be copied
 * @return 0 on success, -1 or other non-zero value on error
 */
int ksyscall_proc_get_name(char *name) {
    if(!active_proc || !name){
    return -1;
    }
    strncpy(name, active_proc->name, PROC_NAME_LEN);                                                                                           return 0; //success 
}
