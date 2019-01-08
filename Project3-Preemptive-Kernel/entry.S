#include "asm.h"
#include "regs.h"
#include "cp0regdefs.h"

  /* Hardcoded values. Do not change these!! */
  /* Offset of the kernel context in a PCB */
  .equ  KERNEL, 8
  /* Offset of the user context in a PCB */
  .equ  USER, 164
  /* Offset of %esp in a context */
  .equ  STACK, TF_REG29
  /* Offset of field nested_count in a PCB */
  .equ  NESTED_COUNT, 320

.macro PUTCHAR ch label
	  addi  k0,$0,10000
	\label:
	addi  k0,k0,-1
	bne k0,$0,\label

	addi  k0,$0,\ch
	la    k1,0xbfe48000
	sb    k0,0(k1)
.endm

.macro STI/*??change the lowest bit to 1 of CP0_STATUS, that means enable interruptSTI是允许外部中断*/
  mfc0    k0, CP0_STATUS
  li      k1, (STATUS_CU0 | 0x1)
  or      k0, k1
  mtc0    k0, CP0_STATUS
.endm

.macro CLI/*??change the lowest bit to 0 of CP0_STATUS??CLI是屏蔽外部中断*/
  mfc0    k0, CP0_STATUS
  li      k1, (STATUS_CU0 | 0x1)
  or      k0, k1
  xori    k0, 0x1
  mtc0    k0, CP0_STATUS
.endm

/* Do not change any of these macros! */
/* Save registers/flags to the specified offset in the current PCB */
.macro SAVE_CONTEXT offset
/* TODO: need add */
  la       k1, current_running
  lw       k0, 0(k1)
  addi     k0, k0, \offset

  sw       AT, 4(k0)
  sw       v0, 8(k0)
  sw       v1, 12(k0)
  sw       a0, 16(k0)
  sw       a1, 20(k0)
  sw       a2, 24(k0)
  sw       a3, 28(k0)
  sw       t0, 32(k0)
  sw       t1, 36(k0)
  sw       t2, 40(k0)
  sw       t3, 44(k0)
  sw       t4, 48(k0)
  sw       t5, 52(k0)
  sw       t6, 56(k0)
  sw       t7, 60(k0)
  
  sw       s0, 64(k0)
  sw       s1, 68(k0)
  sw       s2, 72(k0)
  sw       s3, 76(k0)
  sw       s4, 80(k0)
  sw       s5, 84(k0)
  sw       s6, 88(k0)
  sw       s7, 92(k0)
  sw       t8, 96(k0)
  sw       t9, 100(k0)
  
  sw       gp, 112(k0)
  
  sw      sp, 116(k0)
  sw      $30,120(k0)  
  sw      ra, 124(k0)

  mfc0     k1, CP0_STATUS
  sw       k1, 128(k0)
  mfhi     k1
  sw       k1, 132(k0)
  mflo     k1
  sw       k1, 136(k0)
  mfc0     k1, CP0_BADVADDR
  sw       k1, 140(k0)
  mfc0     k1, CP0_CAUSE
  sw       k1, 144(k0)
  mfc0     k1, CP0_EPC
  sw       k1, 148(k0)
/***************** ending **************/

/**************************
之前认为对 sp 与 ra 的保存要根据是线程调用 do_*** 还是
进程被中断分类讨论的，后来看 kernel 文件反汇编发现
不需要这样处理
  mfc0	   k0, CP0_CAUSE
  andi     k0, k0, CAUSE_IPL
  andi     k0,k0,0x8000
  bnez     k0, 1f
  nop
  j 2f
  
1:
  sw      sp, 116(t0) 
  sw      ra, 124(t0)

   j 3f
  nop

2:
  lw       t1,  24(sp)
  sw       t1,  124(t0)
  addi     sp,  sp, 32
  sw       sp,  116(t0)
  addiu    sp, sp, -32
  j  3f
  nop
3:
	nop  
  
 ***************************/
  
/* TODO: end */
.endm

/* Restore registers/flags from the specified offset in the current PCB */
.macro RESTORE_CONTEXT offset
/* TODO: need add */
	
	la	k1, current_running
	lw  k0, 0(k1)
	addi k0, k0, \offset
  
  lw       AT, 4(k0)
  lw       v0, 8(k0)
  lw       v1, 12(k0)
  lw       a0, 16(k0)
  lw       a1, 20(k0)
  lw       a2, 24(k0)
  lw       a3, 28(k0)
  lw       t0, 32(k0)
  lw       t1, 36(k0)
  lw       t2, 40(k0)
  lw       t3, 44(k0)
  lw       t4, 48(k0)
  lw       t5, 52(k0)
  lw       t6, 56(k0)
  lw       t7, 60(k0)
	
  lw       s0, 64(k0)
  lw       s1, 68(k0)
  lw       s2, 72(k0)
  lw       s3, 76(k0)
  lw       s4, 80(k0)
  lw       s5, 84(k0)
  lw       s6, 88(k0)
  lw       s7, 92(k0)
  
  lw       t8, 96(k0)
  lw       t9, 100(k0)
  
  lw       gp, 112(k0)
  
  lw      sp, 116(k0)
  lw      $30,120(k0)  
  lw      ra, 124(k0)
  
  lw      k1,128(k0)
  mtc0    k1, CP0_STATUS

  lw       k1, 132(k0) 
  mthi     k1

  lw       k1, 136(k0)  
  mtlo     k1

  lw       k1, 140(k0)  
  mtc0     k1, CP0_BADVADDR

  lw       k1, 144(k0)
  mtc0     k1, CP0_CAUSE
 
  lw       k1, 148(k0)
  mtc0     k1, CP0_EPC

/* TODO: end */
.endm

/* Increment disable_count, panicking on a double ENTER
 * Interrupts should be disabled before this macro is used
 */
.macro ENTER_CRITICAL
  la      k0, disable_count
  lw      k1, 0(k0)
//  li      k1, 0/************** 用于测试 *************************/
  bne     k1, $0, panice
  nop
  addi    k1, k1, 1
  sw      k1, 0(k0)
.endm

/* Decrement disable_count, panicking on a double LEAVE
 * Interrupts should be enabled after this macro is used
 */
.macro LEAVE_CRITICAL
  la      k0, disable_count
  lw      k1, 0(k0)            /**put long number in addr(disable_count) to k1*/
  addi    k1, k1, -1
//  li      k1, 0/******************** 用于测试 **********/
  bne     k1, $0, panicl
  nop
  sw      k1, 0(k0)
.endm

/* TODO: Test current_running->nested_count, clobbering only eflags */
.macro TEST_NESTED_COUNT
	/*检查nested_count是否为0*/
.endm

/** TODO end */

  .data
// Temporary storage for 32-bit registers
scratch:
  .long    0
// Temporary storage for 32-bit registers
scratch2:
  .long    0
// Temporary storage for syscall return value
return_value:
  .long    0
// Signals status of interrupts
    .globl    disable_count
disable_count:
  .long    0 /************************** long -> word 没有影响*********************************************************/

  .text

NESTED(scheduler_entry,0,ra)
	/* TODO: need add */
	/*先调用 SAVE_CONTEXT(KERNEL)保存内核上下文
	再调用 scheduler 去找到新进程
	恢复上下文 RESTORE_CONTEXT(KERNEL)且进入到新进程入口地址*/	

	SAVE_CONTEXT(KERNEL)
	jal scheduler
	nop 
	
	LEAVE_CRITICAL
	RESTORE_CONTEXT(KERNEL)
	STI

/************************ wasted *************************************/
//本来想要对第一次被恢复的进程进行特殊处理，使其
//跳转到 handle_int 后半部分去清一下中断，后来发现
//不这样做也能跑
	
//第一次被恢复回来的后一个进程需要清一下中断 pid = 3
/*	la t0, current_running
	lw t0, 0(t0)
	lw t0,336(t0) //current_running->status
	
	la t1, current_running
	lw t1, 0(t1)
	lw t1, 332(t1)

	or t0,t0,t1
	
	// if current_running->status == FIRST_TIME (0)  需要清中断
	beq  t0,zero, clear_int_and_eret  //若不为0，则不需要清中断，跳转走
	nop
*/	
	//清中断
/*	mfc0 k0, CP0_CAUSE
	li  k1, 0xffff00ff
	and k0, k0, k1
    mtc0 k0, CP0_CAUSE
	nop
1:*/
/************************* wasted ending ***********************************/
	jr ra
	nop

	/* TODO: end */
END(scheduler_entry)

LEAF(asm_start)
  /* Disable interrupts */
  mtc0    zero, CP0_STATUS

  /* Disable watch exception. */
  mtc0    zero, CP0_WATCHLO
  mtc0    zero, CP0_WATCHHI

  /* disable kernel mode cache */
  mfc0    t0, CP0_CONFIG
  and     t0, ~0x7
  ori     t0, 0x2
  mtc0    t0, CP0_CONFIG

  jr  ra
END(asm_start)

LEAF(reset_timer)
  mtc0 $0, CP0_COUNT
  mtc0 a0, CP0_COMPARE
  jr ra
END(reset_timer)

  .global exception_handler_start
  .global exception_handler_end
NESTED(general_exception_handler,0,sp)
exception_handler_start:
  .set  noat
  .set  mips32

  CLI

  mfc0  k0, CP0_CAUSE
  andi  k0, k0, CAUSE_EXCCODE   /* k0 = {exc_code,00} */
  la    k1, interrupt_handlers
  add   k0, k0, k1
  lw    k0, 0(k0)
  jr  k0     /* interrupt_handlers[exc_code](); */
exception_handler_end:
END(general_exception_handler)
  .set at
  .set reorder
  .text

LEAF(return_from_exception)
	STI

	eret
END(return_from_exception)

NESTED(simple_handler,0,sp)
  SAVE_CONTEXT(KERNEL)

  mfc0  k0, CP0_CAUSE           /* Read Cause register for IP bits */
  //mfc0  k1, CP0_STATUS        /* and Status register for IM bits */
  andi  k0, k0, CAUSE_IPL       /* Keep only IP bits from Cause */
  //and   k0, k0, k1            /* and mask with IM bits */
  clz   k0, k0                  /* Find first bit set, IP7..IP0; k0 = 16..23 */
  xori  k0, k0, 0x17            /* 16..23 => 7..0 */
  add   a0, k0, $0

  mfc0  k0, CP0_CAUSE
	andi  k0, k0, CAUSE_EXCCODE   /* k0 = {exc_code,00} */
  srl   k0, k0, 2
  add   a1, k0, $0

  jal   c_simple_handler

  mfc0  k0, CP0_CAUSE           /* Read Cause register for IP bits */
	andi  k1, k0, CAUSE_IPL       /* Keep only IP bits from Cause */
	xor   k0, k0, k1
  mtc0  k0, CP0_CAUSE

  LEAVE_CRITICAL
  RESTORE_CONTEXT(KERNEL)
  j return_from_exception
END(simple_handler)

NESTED(handle_syscall,0,sp)
  SAVE_CONTEXT(USER)
  //RESTORE_CONTEXT(KERNEL)
  add   a3,a2,$0
  add   a2,a1,$0
  add   a1,a0,$0
  add   a0,v0,$0

  jal system_call_helper

  LEAVE_CRITICAL
  RESTORE_CONTEXT(USER)
  j return_from_exception
END(handle_syscall)

NESTED(handle_int,0,sp)
  /* TODO: timer_irq */
  /* read int IP and handle clock interrupt or just call do_nothing */
/*******************old*************************/ 
/*老版本按照任务书上的写，没有用写 count 和 compare 寄存器带来的
特殊效果，后来知道写 count 与 compare 寄存器 与采用 eret 后 MIPS
的 CPU 会自动执行某些清时钟中断的操作
SAVE_CONTEXT(USER)
  mfc0 k0, CP0_CAUSE
  andi k0, k0, CAUSE_IPL
  andi k0,k0,0x8000
  bnez k0, K1
  nop 
  j K2

K1:  
  jal  timer_irq
  nop
  mfc0 k0, CP0_CAUSE
  and k0, k0, 0xffff00ff
  mtc0 k0, CP0_CAUSE
  jal scheduler_entry
  nop

K2:
  mfc0 k0, CP0_CAUSE
  and k0, k0, 0xffff00ff
  mtc0 k0, CP0_CAUSE
  
//  jal leave_critical
  nop
  RESTORE_CONTEXT(USER)
*/  
  
/********************** new *******************************/
 
 SAVE_CONTEXT(USER)
   mfc0 k0, CP0_CAUSE
   andi k0, k0, CAUSE_IPL//CAUSE_IPL 0xff00:Keep only IP bits from Cause
   andi k0,k0,0x8000   //检查时钟中断
   bnez k0, 1f        //若是时钟中断，则跳转到 1f 处处理
   nop
   j  2f             //若不是时钟中断，则跳转到 2f 处处理
   nop
   

1: 
    mtc0 zero, CP0_COUNT
	li k0, 150000000
	mtc0 k0,CP0_COMPARE


	jal  timer_irq  //是时钟中断，则跳转到 timer_irq 处。
	nop
	
2: //清中断，线程与进程都要干
	
	mfc0 k0, CP0_CAUSE
	li   k1, 0xffff00ff
	and  k0, k0, k1
    mtc0 k0, CP0_CAUSE
	nop


    RESTORE_CONTEXT(USER)
	STI
	nop
	eret
	nop

  /* TODO:end */
END(handle_int)

ret:
  LEAVE_CRITICAL   // del disable_count
  eret

LEAF(get_cp0_status)
  mfc0  v0, CP0_STATUS
  jr ra
END(get_cp0_status)

LEAF(set_cp0_status)
  mtc0  a0, CP0_STATUS
  jr ra
END(set_cp0_status)


LEAF(enter_critical)
  CLI
  ENTER_CRITICAL
  jr ra
END(enter_critical)

LEAF(leave_critical)
  LEAVE_CRITICAL
  STI
  jr ra
END(leave_critical)

/* Error Message: Write "PANIC" to the screen and enter an infinite loop */
LEAF(panic)
  CLI
  PUTCHAR 0x50 P1// 'P'
  j panic
END(panic)
LEAF(panice)
  CLI
  PUTCHAR 69 P2// 'E'
  j panic
END(panice)
	LEAF(panicl)
	  CLI
	  PUTCHAR 76 P3// 'L'
	  j panic
	END(panicl)
//  cli
//  movl    $0xb800a, %esp
//  pushw    $0x4f43
//  pushw    $0x4f49
//  pushw    $0x4f4e
//  pushw    $0x4f41
//  pushw    $0x4f50
LEAF(infinite_loop)
//        hlt
  j    infinite_loop
END(infinite_loop)
