section .text

global mt_tasking_switchTask
global mt_tasking_switchTaskInitial

mt_tasking_switchTask:
	
	mov eax, [ esp + 4 ]
	mov ecx, [ esp + 8 ]
	
	push ebp
	push ebx
	push esi
	push edi
	
	mov [ eax ], esp
	mov esp, edx
	
	pop edi
	pop esi
	pop ebx
	pop ebp
	
	ret
	