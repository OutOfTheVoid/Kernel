
global hw_cpu_idtLoad

extern Kernel_IDTR

segment .text

hw_cpu_idtLoad:
	
	lidt [ Kernel_IDTR ]
	ret
