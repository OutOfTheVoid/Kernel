section .text

global mt_synchronization_tryslock
global mt_synchronization_pwacquirelock
global mt_synchronization_release

mt_synchronization_tryslock:

	push ebp
	mov ebp, esp
	
	mov eax, [ esp + 8 ]
	
	lock bts dword [ eax ], 1
	
	jc __tryslock_fail
	mov eax, 1
	
	mov esp, ebp
	pop ebp
	
	ret

__tryslock_fail:

	mov eax, 0
	
	mov esp, ebp
	pop ebp
	
	ret
	

mt_synchronization_pwacquirelock:

	push ebp
	mov ebp, esp
	
	mov eax, [ esp + 8 ]
	
__pwacquirelock_retry:

	pause
	lock bts dword [ eax ], 1
	jc __pwacquirelock_retry
	
	mov esp, ebp
	pop ebp
	
	ret
	
mt_synchronization_release:
	
	push ebp
	mov ebp, esp
	
	mov eax, [ esp + 8 ]
	
	mov dword [ eax ], 0
	
	mov esp, ebp
	pop ebp
	
	ret
