#include "regs.h"
	.text
	.align	2
	.globl  get_timer	
	.ent	get_timer
	.type	get_timer, @function
get_timer:
	mfc0 $2,$9
        nop
        j       ra
        nop
	.align	2
	.globl  kernel_entry	
	.ent	kernel_entry
	.type	kernel_entry, @function
kernel_entry:
	addiu sp, sp, -24
	sw ra, 0(sp)
	bnez $4, 1f
	nop

	jal do_yield
	nop
	beqz $0,2f
	nop

1:
	jal do_exit
	nop
2:
	lw ra, 0(sp)
	addiu sp, sp, 24
	jr ra
	nop

	.align	2
	.globl  scheduler_entry	
	.ent	scheduler_entry
	.type	scheduler_entry, @function
scheduler_entry:
	# call scheduler, which will set new current process
	# need student add
	jal scheduler
	nop
	
	lw $27, current_running
	
	lw s0, 0($27)	
	lw s1, 4($27)	
	lw s2, 8($27)	
	lw s3, 12($27)	
	lw s4, 16($27)	
	lw s5, 20($27)	
	lw s6, 24($27)	
	lw s7, 28($27)	
	lw s8, 32($27) /* == fp */
	lw ra, 36($27)
	lw sp, 40($27)
	
	jr $31
	nop
	
	.align	2
	.globl  save_pcb	
	.ent	save_pcb
	.type	save_pcb, @function
save_pcb:
	# save the pcb of the currently running process
	# need student add
	lw $27, current_running
	lw $26, 24(sp)
	addiu sp, sp, 32
	sw s0, 0($27)	
	sw s1, 4($27)	
	sw s2, 8($27)	
	sw s3, 12($27)	
	sw s4, 16($27)	
	sw s5, 20($27)	
	sw s6, 24($27)	
	sw s7, 28($27)	
	sw s8, 32($27)
	sw $26, 36($27)
	sw  sp ,40($27)
	
	
	jr $31
	nop

	
	
