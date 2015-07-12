
MBALIGN		equ	1<<0				; Align loaded modules on page boundaries
MEMINFO		equ 1<<1				; Provide memory map
VIDINFO		equ 1<<2				; Provide video info

GRLINEAR	equ 0
GRTEXT		equ 1<<0

GRTEXTH		equ 24
GRTEXTW		equ 80
GRDEPTH		equ 0

FLAGS		equ	MBALIGN | MEMINFO | VIDINFO	; Multiboot Flags
MAGIC		equ	0x1BADB002			; Multiboot Magic Number
CHECKSUM	equ -(MAGIC + FLAGS)	; Checks

; --------------- Multiboot header -------------------------- ;

section .multiboot
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM
	times 5 dd 0
	dd GRTEXT
	dd GRTEXTW
	dd GRTEXTH
	dd GRDEPTH



global _start
global boot_multiboot_StackTop
global boot_multiboot_StackBottom

extern hw_cpu_hang
extern init_kinit_kinit

; ------------------- Boot stack ---------------------------- ;

%define BOOT_STACK_SIZE 0x20000

section .boot_stack

align 4

boot_multiboot_StackBottom:
	times BOOT_STACK_SIZE db 0
boot_multiboot_StackTop:

; ------------------- _start (Entry) ------------------------ ;

section .text
_start:

	cli

	mov esp, boot_multiboot_StackTop

	push DWORD 0
	popf

	push DWORD ebx
	push DWORD eax
	
	call init_kinit_kinit

	jmp hw_cpu_hang

