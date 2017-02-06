section .text

global mt_tasking_switchTask
global mt_tasking_switcher_TaskListLockRef

extern mt_synchronization_release

mt_tasking_switcher_TaskListLockRef:

	dd 0x00000000

mt_tasking_switchTask:
	
	mov eax, [ esp + 4 ]
	mov ecx, [ esp + 8 ]
	
	push ebp
	push ebx
	push esi
	push edi
	
	mov [ eax ], esp
	mov esp, ecx
	
	mov eax, [ mt_tasking_switcher_TaskListLockRef ]
	mov dword [ eax ], 0
	
	pop edi
	pop esi
	pop ebx
	pop ebp
	
	ret
	