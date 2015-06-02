
global mm_segmentation_gdtLoad

extern Kernel_GDTR
extern Kernel_OLD_GDTR

segment .text

mm_segmentation_gdtLoad:

	;sgdt [ Kernel_OLD_GDTR ]
	lgdt [ Kernel_GDTR ]
	
	jmp 0x08:__gdt_flush
	
__gdt_flush:

	mov ax, 0x10
	
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	
	ret
	