global __mt_apinit_trampolinestart
global __mt_apinit_trampolineend

section .data

align 4

__mt_apinit_trampolinestart:

	INCBIN "APTrampoline.bin"
	
__mt_apinit_trampolineend:
	
	