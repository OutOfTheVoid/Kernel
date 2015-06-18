section .text

global interrupt_ISRCommonHandler
global interrupt_IRQCommonHandler

extern interrupt_InterruptHandlers_CommonISRHandler
extern interrupt_IRQ_CommonIRQHandler

%macro ISR_NOERRORCODE 1

	global interrupt_ISR%1

	interrupt_ISR%1:
		
		push 0
		push %1
		
		jmp interrupt_ISRCommonHandler

%endmacro

%macro ISR_ERRORCODE 1

	global interrupt_ISR%1

	interrupt_ISR%1:
		
		push %1
		
		jmp interrupt_ISRCommonHandler

%endmacro

%macro IRQ 1

	global interrupt_IRQ%1
	
	interrupt_IRQ%1:
		
		push %1
		
		jmp interrupt_IRQCommonHandler
		
%endmacro


ISR_NOERRORCODE 0x00
ISR_NOERRORCODE 0x01
ISR_NOERRORCODE 0x02
ISR_NOERRORCODE 0x03
ISR_NOERRORCODE 0x04
ISR_NOERRORCODE 0x05
ISR_NOERRORCODE 0x06
ISR_NOERRORCODE 0x07
ISR_ERRORCODE 0x08
ISR_NOERRORCODE 0x09
ISR_ERRORCODE 0x0A
ISR_ERRORCODE 0x0B
ISR_ERRORCODE 0x0C
ISR_ERRORCODE 0x0D
ISR_ERRORCODE 0x0E
ISR_NOERRORCODE 0x0F
ISR_NOERRORCODE 0x10
ISR_ERRORCODE 0x11
ISR_NOERRORCODE 0x12
ISR_NOERRORCODE 0x13
ISR_NOERRORCODE 0x14
ISR_NOERRORCODE 0x15
ISR_NOERRORCODE 0x16
ISR_NOERRORCODE 0x17
ISR_NOERRORCODE 0x18
ISR_NOERRORCODE 0x19
ISR_NOERRORCODE 0x1A
ISR_NOERRORCODE 0x1B
ISR_NOERRORCODE 0x1C
ISR_NOERRORCODE 0x1D
ISR_ERRORCODE 0x1E
ISR_NOERRORCODE 0x1F

IRQ 0x00
IRQ 0x01
IRQ 0x02
IRQ 0x03
IRQ 0x04
IRQ 0x05
IRQ 0x06
IRQ 0x07
IRQ 0x08
IRQ 0x09
IRQ 0x0A
IRQ 0x0B
IRQ 0x0C
IRQ 0x0D
IRQ 0x0E
IRQ 0x0F
IRQ 0x10
IRQ 0x11
IRQ 0x12
IRQ 0x13
IRQ 0x14
IRQ 0x15
IRQ 0x16
IRQ 0x17
IRQ 0x18
IRQ 0x19
IRQ 0x1A
IRQ 0x1B
IRQ 0x1C
IRQ 0x1D
IRQ 0x1E
IRQ 0x1F

IRQ 0xFF

interrupt_ISRCommonHandler:

	pushad
	
	mov ax, ds
	push eax
	
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	call interrupt_InterruptHandlers_CommonISRHandler
	
	pop eax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	popad
	
	add esp, 8
	
	sti
	
	iret
	
	
interrupt_IRQCommonHandler:

	pushad
	
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	call interrupt_IRQ_CommonIRQHandler
	
	pop eax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	popad
	
	add esp, 4
	
	iret
	
	
