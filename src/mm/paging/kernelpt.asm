
global mm_paging_kernelpt
global mm_paging_kernelpd

section .page_table

align 4096

mm_paging_kernelpd:

	times 0x1000 db 0

align 4096
	
mm_paging_kernelpt:
	
	times 0x400000 db 0