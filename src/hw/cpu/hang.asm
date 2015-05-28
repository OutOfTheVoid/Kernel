
global hw_cpu_hang

; ------------------- void hw_cpu_hang () ---------------------- ;

section .text
hw_cpu_hang:
	
	hlt
	jmp hw_cpu_hang
