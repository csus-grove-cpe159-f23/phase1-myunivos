/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 *
 * System call kernel handlers
 */
#ifndef KSYSCALL_H
#define KSYSCALL_H

#include "syscall_common.h"

/**
 * System call IRQ handler
 * Dispatches system calls to the function associate with the specified system call
 */
void ksyscall_irq_handler(void);

/**
 * System Call Initialization
 */
void ksyscall_init(void);

/**
 * Writes up to n bytes to the process' specified IO buffer
 * @param io - the IO buffer to write to
 * @param buf - the buffer to copy from
 * @param n - number of bytes to write
 * @return -1 on error or value indicating number of bytes copied
 */
int ksyscall_io_write(int io, char *buf, int n);

/**
 * Reads up to n bytes from the process' specified IO buffer
 * @param io - the IO buffer to read from
 * @param buf - the buffer to copy to
 * @param n - number of bytes to read
 * @return -1 on error or value indicating number of bytes copied
 */
int ksyscall_io_read(int io, char *buf, int n);

/**
 * Flushes (clears) the specified IO buffer
 * @param io - the IO buffer to flush
 * @return -1 on error or 0 on success
 */
int ksyscall_io_flush(int io);

/**
 * Gets the current system time (in seconds)
 * @return system time in seconds
 */
int ksyscall_sys_get_time(void);

/**
 * Gets the operating system name
 * @param name - pointer to a character buffer where the name will be copied
 * @return 0 on success, -1 or other non-zero value on error
 */
int ksyscall_sys_get_name(char *name);

/**
 * Puts the current process to sleep for the specified number of seconds
 * @param seconds - number of seconds the process should sleep
 */
int ksyscall_proc_sleep(int seconds);

/**
 * Exits the current process
 */
int ksyscall_proc_exit(void);

/**
 * Gets the current process' id
 * @return process id
 */
int ksyscall_proc_get_pid(void);

/**
 * Gets the current process' name
 * @param name - pointer to a character buffer where the name will be copied
 * @return 0 on success, -1 or other non-zero value on error
 */
int ksyscall_proc_get_name(char *name);


#endif

