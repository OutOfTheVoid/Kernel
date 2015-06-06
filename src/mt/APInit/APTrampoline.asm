[BITS 16]
[ORG 0x1000]

%macro SEGMENT_NULL 0
	
	dw 0x0000
	dd 0x00000000
	
%endmacro
	
%macro SEGMENT_32 3 ;(type, base, limit)

	dw ( ( ( %3 ) >> 12 ) & 0xFFFF ), ( ( %2 ) & 0xFFFF )
	db ( ( ( %2 ) >> 16 ) & 0xFF ), ( 0x90 | ( %1 ) ), ( 0xC0 | ( ( ( %3 ) >> 28 ) & 0x0F ) ), ( ( ( %2 ) >> 24 ) & 0xFF )
	
%endmacro

%define SEGMENT_TYPE_EXECUTABLE 0x08
%define SEGMENT_TYPE_D_EXPANDDOWN 0x04
%define SEGMENT_TYPE_C_CONFORMING 0x04
%define SEGMENT_TYPE_D_WRITABLE 0x02
%define SEGMENT_TYPE_C_READABLE 0x02
%define SEGMENT_TYPE_ACCESSED 0x01
	
%define CR0FLAG_PROTECTEDMODE 0x01

mt_apinit_kapmain:

	dd 0

mt_apinit_initstacktop:

	dd 0
	
mt_apinit_startptr:

	dd mt_apinit_trampoline

mt_apinit_gdt:

	SEGMENT_NULL
	SEGMENT_32 ( SEGMENT_TYPE_EXECUTABLE | SEGMENT_TYPE_C_READABLE ), 0, 0xFFFFFFFF
	SEGMENT_32 SEGMENT_TYPE_D_WRITABLE, 0, 0xFFFFFFFF
	
mt_apinit_gdtend:

mt_apinit_gdtr:

	dw ( mt_apinit_gdtend - mt_apinit_gdt - 1 )
	dd mt_apinit_gdt

mt_apinit_trampoline:

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
	mov ss, ax
	
	mov ax, 0x00
	mov fs, ax
	mov gs, ax
	
	mov eax, [ mt_apinit_initstacktop ]
	mov esp, eax
	
	call [mt_apinit_kapmain]
	
mt_apinit_hang:
	
	cli
	hlt
	jmp mt_apinit_hang
