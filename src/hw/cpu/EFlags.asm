section .text

global hw_cpu_readEFlags

hw_cpu_readEFlags:

	pushfd
	pop eax
	
	ret