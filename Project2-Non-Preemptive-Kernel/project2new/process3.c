#include "common.h"
#include "syslib.h"
#include "util.h"

void _start(void)
{
	/* need student add */

	test_time = get_timer() - current_time;
	print_location(0,2);
	printstr("thread to process(time)");
//	print_str(2, 0, string);
    int ctime;
	ctime = test_time / MHZ;
//	print_location(40,50);
	printint(30,2,ctime);
//	print_int(2, 10, test_time);
	
	yield();
	while(1)
		yield();
}

