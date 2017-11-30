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
	
	lui  $8, 0xbfe4
	ori $8, 0x8000
	li  $9, 0x23
	sb  $9, ($8)
	
	nop
	li  $9, 0x23
	sb  $9, ($8)
	
	nop
	li  $9, 0x23
	sb  $9, ($8)	

	nop
	li  $9, 0x25
	sb  $9, ($8)

	jr $31	
		
	
	#need add code
	# call printstr print a string

