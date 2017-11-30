#include "common.h"
#include "scheduler.h"
#include "util.h"

char * string = "thread: ";

void thread4(void)
{
	current_time = get_timer();//test_time defined in common.h
	do_yield();
	do_exit();
}

void thread5(void)
{
	test_time = get_timer() - current_time;
//	print_str(0, 0, string);
    print_location(0,0);
	printstr("thread to thread:");
	int ctime;
	ctime = test_time / MHZ;
//	print_int(0, 10, test_time);
	printint(30,0,ctime);
	current_time = get_timer();
	do_yield();
	do_exit();
}
