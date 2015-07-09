section .text

global mt_tasking_SwitchKernelStack
global mt_tasking_switchTaskInitial

;void * mt_tasking_SwitchTask ( void * OldProcesseStack, void * NewProcessStack )

mt_tasking_switchTask:
	
	push ebp
	mov ebp, esp
	
	push ebx
	push esi
	push edi
	
	mov ecx, [ ebp + 8 ]
	mov eax, esp
	mov [ ecx ], eax
	
	mov eax, [ ebp + 12 ]
	mov esp, eax
	
	pop edi
	pop esi
	pop ebx
	pop ebp
	
	ret
	
mt_tasking_switchTaskInitial:

	pop eax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	popad
	
	add esp, 8
	
	iret
	