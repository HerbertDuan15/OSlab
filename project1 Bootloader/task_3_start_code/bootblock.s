    .data
num: 
    .word 0x0000000
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

    la $8,num
    lw $9,($8)
	
	li $4, 0xa0800200
	li $5, 0x200
	li $6, 0x0
lod:
    beq $0,$9,ed
    addiu $6,$6,0x200
	addiu $9,$9,-1
	j lod

ed:	
	jal 0x8007b1a8
	jal 0xa080026c
	
l1:
	nop
    j l1	

	#need add code
	#read kernel

