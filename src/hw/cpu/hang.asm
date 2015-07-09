; ------------------- void hw_cpu_hang () ---------------------- ;

section .text

global hw_cpu_hang

hw_cpu_hang:
	
	hlt
	jmp hw_cpu_hang
	
