/*
   kernel.c
   the start of kernel
   */

#include "common.h"
#include "kernel.h"
#include "scheduler.h"
#include "th.h"
#include "util.h"
#include "queue.h"

#include "tasks.c"


volatile pcb_t current_running;
struct queue ready_queue_init;
struct queue blocked_queue_init;

queue_t ready_queue; 
queue_t blocked_queue;

pcb_t ready_arr[NUM_TASKS];
pcb_t blocked_arr[NUM_TASKS];
struct pcb pcbs[NUM_TASKS];

/*
   this function is the entry point for the kernel
   It must be the first function in the file
   */

#define PORT3f8 0xbfe48000

 void printnum(unsigned long long n)
 {
   int i,j;
   unsigned char a[40];
   unsigned long port = PORT3f8;
   i=10000;
   while(i--);

   i = 0;
   do {
   a[i] = n % 16;
   n = n / 16;
   i++;
   }while(n);

  for (j=i-1;j>=0;j--) {
   if (a[j]>=10) {
      *(unsigned char*)port = 'a' + a[j] - 10;
    }else{
	*(unsigned char*)port = '0' + a[j];
   }
  }
  printstr("\r\n");
}

void init_queue()
{
	//ready_queue
	ready_queue = &ready_queue_init;
	queue_init(ready_queue); 
	ready_queue->capacity = NUM_TASKS;
	ready_queue->pcbs = ready_arr; 	
	printstr("\tinit ready_queue ended\r\n");	
	//blocked_queue
	blocked_queue = &blocked_queue_init;
	queue_init(blocked_queue);
	blocked_queue->capacity = NUM_TASKS;
	blocked_queue->pcbs = blocked_arr;
	printstr("\tinit blocked_queue ended\r\n");
}


void init_pcb(int num) 
{
	ready_arr[num]->state = PROCESS_READY;
	ready_arr[num]->pid = num;
	ready_arr[num]->ps0 = 0x0;
	ready_arr[num]->ps1 = 0x0;
	ready_arr[num]->ps2 = 0x0;
	ready_arr[num]->ps3 = 0x0;
	ready_arr[num]->ps4 = 0x0;
	ready_arr[num]->ps5 = 0x0;
	ready_arr[num]->ps6 = 0x0;
	ready_arr[num]->ps7 = 0x0;
	ready_arr[num]->ps8 = 0x0;          /* == fp */
	ready_arr[num]->pra = task[num]->entry_point;
	ready_arr[num]->psp = STACK_MIN+num*STACK_SIZE;
}


void init_task(int num){
	
	init_pcb(num);
	queue_push(ready_queue,ready_arr[num]);
	printstr("\t init task "); printnum((unsigned long long)num); printstr(" ended\r\n");
//	printnum((unsigned long long)task_pcb->kstack_ptr); printstr("\r\n");
}

void _stat(void){

	/* some scheduler queue initialize */
	/* need student add */
	init_queue();	

	
	int i;
	for (i=0;i<NUM_TASKS;i++) ready_arr[i] = &pcbs[i];
	for (i=0;i<NUM_TASKS;i++) init_task(i);
	
	printstr("init_all done\r\n");
	clear_screen(0, 0, 30, 24);
	/* Initialize the PCBs and the ready queue */
	/* need student add */
	
	


	/*Schedule the first task */
	scheduler_count = 0;
	scheduler_entry();

	/*We shouldn't ever get here */
	ASSERT(0);
} 