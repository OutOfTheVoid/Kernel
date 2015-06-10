
global mm_paging_loadPageDirectory
global mm_paging_enable
global mm_paging_disable
global mm_paging_flushCR3
global mm_paging_invalPage
global mm_paging_getStatus

extern Current_PDirectory

section .text

mm_paging_loadPageDirectory:
	
	push ebp
	mov ebp, esp
	
	mov eax, [ esp + 8 ]
	mov cr3, eax
	
	mov esp, ebp
	pop ebp
	
	ret
	
mm_paging_flushCR3:

	push ebp
	mov ebp, esp
	
	mov eax, cr3
	mov cr3, eax
	
	mov esp, ebp
	pop ebp
	
	ret
	
mm_paging_enable:
	
	push ebp
	mov ebp, esp
	
	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax
	
	mov esp, ebp
	pop ebp
	
	ret
	
mm_paging_disable:

	push ebp
	mov ebp, esp
	
	mov eax, cr0
	and eax, ~ 0x80000000
	mov cr0, eax
	
	mov esp, ebp
	pop ebp
	
	ret
	
mm_paging_invalPage:

	push ebp
	mov ebp, esp
	
	mov eax, [ esp + 8 ]
	invlpg [eax]
	
	mov esp, ebp
	pop ebp
	
	ret
	
mm_paging_getStatus:

	push ebp
	mov ebp, esp

	mov eax, cr0
	and eax, 0x80000000
	
	mov esp, ebp
	pop ebp
	
	ret
	