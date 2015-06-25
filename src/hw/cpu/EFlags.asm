section .text

global hw_cpu_readEFlags

hw_cpu_readEFlags:

	popfd
	push eax
	
	ret