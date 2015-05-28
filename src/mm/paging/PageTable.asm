
global mm_paging_loadPageDirectory
global mm_paging_enable
global mm_paging_disable
global mm_paging_flushCR3

section .text

mm_paging_loadPageDirectory:
	
	mov eax, [ esp + 8 ]
	mov cr3, eax
	
	ret
	
mm_paging_flushCR3:

	mov eax, cr3
	mov cr3, eax
	
	ret
	
mm_paging_enable:
	
	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax
	
	ret
	
mm_paging_disable:

	mov eax, cr0
	and eax, ~ 0x80000000,
	mov cr0, eax
	
	ret