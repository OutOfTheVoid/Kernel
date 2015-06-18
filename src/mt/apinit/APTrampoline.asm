[BITS 16]
[ORG 0x1000]

%define CR0FLAG_PROTECTEDMODE 0x01

mt_apinit_trampoline:

	cli
	
	xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    
	lgdt [ mt_apinit_gdtr ]
	
	mov eax, cr0
	or eax, CR0FLAG_PROTECTEDMODE
	mov cr0, eax
	
	jmp 0x08:mt_apinit_protectedjump
	
[BITS 32]
	
mt_apinit_protectedjump:

	mov ax, 0x10
	
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	
	mov eax, [ mt_apinit_initpgdir ]
	mov cr3, eax
	
	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax
	
	mov eax, [ mt_apinit_initstacktop ]
	mov esp, eax
	
	call [ mt_apinit_kapmain ]
	
mt_apinit_hang:
	
	cli
	hlt
	jmp mt_apinit_hang

times 508 - ( $ - $$ ) db 0
	
mt_apinit_signature:

	dd 0x12345678
	
mt_apinit_kapmain:

	dd 0

mt_apinit_initstacktop:

	dd 0
	
mt_apinit_initpgdir:

	dd 0
	
align 2

mt_apinit_gdt:

	dd 0x00000000
	dd 0x00000000

	dw 0xFFFF
	dw 0x0000
	db 0x00
	db 0x9A
	db 0xCF
	db 0x00
	
	dw 0xFFFF
	dw 0x0000
	db 0x00
	db 0x92
	db 0xCF
	db 0x00

mt_apinit_gdtr:

	dw ( mt_apinit_gdtr - mt_apinit_gdt - 1 )
	dd mt_apinit_gdt
