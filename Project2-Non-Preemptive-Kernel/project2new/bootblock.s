	.text
	.globl main

main:
	# check the offset of main
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	
	li		$4,0xa0800200	
	li		$5,0x200
#	li 		$8,0xa0800008
	li		$6,0x3fe00#$6 ← memory[base.$8+offset.0]
#	sll		$6,$6,9#SLL rd, rt, sa   rd ← rt << sa
	
	jal 	0x8007b1a8
    nop  	#jump to read function code
	nop
	jal 	0xa080046c		#jump to kernel 
	nop
	nop
		
		
	#need add code
	#read kernel
l1:
    nop
	j l1
	
