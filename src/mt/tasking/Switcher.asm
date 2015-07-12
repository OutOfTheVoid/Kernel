section .text

global mt_tasking_switchTask
global mt_tasking_switchTaskInitial

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
	