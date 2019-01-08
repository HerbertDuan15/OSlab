/* Author(s): <Your name here>
 * COS 318, Fall 2013: Project 3 Pre-emptive Scheduler
 * Implementation of the process scheduler for the kernel.
 */

#include "common.h"
#include "interrupt.h"
#include "queue.h"
#include "printf.h"
#include "scheduler.h"
#include "util.h"
#include "syslib.h"

#define FIFO  0//改此数字为0则为优先级调度

pcb_t *current_running;
node_t ready_queue;
node_t sleep_wait_queue;
// more variables...
volatile uint64_t time_elapsed;//用于blocking sleep。

/* TODO:wake up sleeping processes whose deadlines have passed */
void check_sleeping(){//唤醒那些当前时间已经达到所需运行时间的task。
	/*检查 sleeping队列中的进程的 deadline，
	把等于这个当前时间的值的task的状态改为ready，
	并放入ready_queue 中 */
	node_t *p;
	pcb_t *temp;
	uint64_t time_now = get_timer();

	for(p = peek(&sleep_wait_queue);p != (&sleep_wait_queue) && p!=NULL; )
	{
		temp = (pcb_t*)p;
		if(temp->deadline <= time_now * 1000)
		{
			temp->status = READY;
			 p=p->next;
			temp->node.prev->next = temp->node.next;
			temp->node.next->prev = temp->node.prev;
			enqueue(&ready_queue, (node_t *)temp);
		}
		else{
			 p=p->next;
		}
	}		
}


/* Round-robin scheduling: Save current_running before preempting */
void put_current_running(){
	/*把当前 task 保存到 ready 队列的末尾。 
	*/
	current_running->status = READY;
	enqueue(&ready_queue,(node_t*)current_running);
//	printf(31,50,"the ready queue is empty? %d",is_empty(&ready_queue));
	
}

/* Change current_running to the next task */
void scheduler(){
     ASSERT(disable_count);
     check_sleeping(); // wake up sleeping processes
     while (is_empty(&ready_queue)){
          leave_critical();
          enter_critical();
          check_sleeping();
     }
	 if(FIFO)
	 {
		 current_running = (pcb_t *) dequeue(&ready_queue);
	 }
	 else{
		 node_t * p = &ready_queue;
	     pcb_t * temp = (pcb_t*)peek(&ready_queue);
		 
	     for(p = peek(p); p != (&ready_queue) && p!=NULL; p=p->next)
		 {
		     if(temp->priority < ((pcb_t*)p)->priority)
			     temp = (pcb_t*)p; //找到优先级最大的任务
		 }
	      //从 ready 对列去掉此任务
	     temp->node.prev->next = temp->node.next;
	     temp->node.next->prev = temp->node.prev;
	     temp->node.next = NULL;
	     temp->node.prev = NULL;
	     current_running = temp;
		 if(current_running->priority > 0)
			 current_running->priority = current_running->priority - 3;
		 else
			 current_running->priority = current_running->priority + 50;
	 }
    
     ASSERT(NULL != current_running);
     ++current_running->entry_count;
//	 ASSERT(disable_count);/************************************/
//	 printf(28,50,"current running pid=%d",(int)current_running->pid);
//	 printf(29,50,"now the current running entrypoint=0x%x",current_running->kernel_tf.regs[31]);
//	 printf(30,50,"now the disable_count=%d",(int)disable_count);
//	 ASSERT(disable_count);/********************************************/
}

int lte_deadline(node_t *a, node_t *b) {
     pcb_t *x = (pcb_t *)a;
     pcb_t *y = (pcb_t *)b;

     if (x->deadline <= y->deadline) {
          return 1;
     } else {
          return 0;
     }
}

void do_sleep(int milliseconds){
     ASSERT(!disable_count);

     enter_critical();
     // TODO
	/*每次都给 deadline 加一个值，
		把此 task 状态改为睡眠，
		把此 task 保存到睡眠队列里。
		调用 scheduler_entry ;
	*/
	current_running->deadline = time_elapsed*1000 + milliseconds;
	current_running->status = SLEEPING;
	enqueue(&sleep_wait_queue,(node_t *)current_running);
	scheduler_entry();
//     leave_critical();
}

void do_yield(){
//	 printf(25,50,"do_yield begin,disable_count=%d",(int)disable_count);
     enter_critical();
//	 printf(26,50,"do_yield begin,disable_count=%d",(int)disable_count);
     put_current_running();
     scheduler_entry();
}

void do_exit(){
     enter_critical();
     current_running->status = EXITED;
     scheduler_entry();
     /* No need for leave_critical() since scheduler_entry() never returns */
}

void block(node_t * wait_queue){
     ASSERT(disable_count);
     current_running->status = BLOCKED;
     enqueue(wait_queue, (node_t *) current_running);
     scheduler_entry();
     enter_critical();
}

void unblock(pcb_t * task){
     ASSERT(disable_count);
     task->status = READY;
     enqueue(&ready_queue, (node_t *) task);
}

pid_t do_getpid(){
     pid_t pid;
     enter_critical();
     pid = current_running->pid;
     leave_critical();
     return pid;
}

uint64_t do_gettimeofday(void){
     return time_elapsed;
}

priority_t do_getpriority(){
	/* TODO *//*选 ready 队列的中优先度高的先进行执行，整合到 scheduler 里*/
	priority_t pri;  
//    enter_critical();
    pri = current_running->priority;
//    leave_critical();
    return pri;
}


void do_setpriority(priority_t priority){
	/* TODO */
//	enter_critical();
    current_running->priority = priority;
//   leave_critical();
}

uint64_t get_timer(void){
     return do_gettimeofday();
}