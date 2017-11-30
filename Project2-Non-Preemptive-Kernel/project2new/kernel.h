/* kernel.h: definitions used by kernel code */

#ifndef KERNEL_H
#define KERNEL_H

#define NUM_REGISTERS 8

#include "common.h"

/* ENTRY_POINT points to a location that holds a pointer to kernel_entry */
#define ENTRY_POINT ((void (**)(int)) 0xa080fff8)


/* System call numbers */
enum {
    SYSCALL_YIELD,
    SYSCALL_EXIT,
};

/* All stacks should be STACK_SIZE bytes large
 * The first stack should be placed at location STACK_MIN
 * Only memory below STACK_MAX should be used for stacks
 */
enum {
    STACK_MIN = 0xa1000000,
    STACK_SIZE = 0x5000,
    STACK_MAX = 0xa1900000,
};

typedef enum {
	PROCESS_BLOCKED,
	PROCESS_READY,
	PROCESS_RUNNING,
	PROCESS_EXITED,
} process_state;


typedef struct pcb{
	uint32_t ps0;
	uint32_t ps1;
	uint32_t ps2;
	uint32_t ps3;
	uint32_t ps4;
	uint32_t ps5;
	uint32_t ps6;
	uint32_t ps7;
	uint32_t ps8;
	uint32_t pra;  
	uint32_t psp;
	/* need student add */
	int pid;	
	int type;
	process_state 	state;
}* pcb_t;

/* The task currently running.  Accessed by scheduler.c and by entry.s assembly methods */
extern volatile pcb_t current_running;
extern int scheduler_count;
void kernel_entry(int fn);

#endif                          /* KERNEL_H */
