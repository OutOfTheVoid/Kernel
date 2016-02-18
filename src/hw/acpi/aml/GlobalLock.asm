section .text

global hw_acpi_aml_globallockacquire
global hw_acpi_aml_globallockrelease

global hw_acpi_aml_globallockaddr

hw_acpi_aml_globallockaddr:
	
	dd 0

; This code is slightly modified from the ACPI spec. version 5

hw_acpi_aml_globallockacquire:
	
	mov ecx, [ hw_acpi_aml_globallockaddr ] ; ecx = Address of Global Lock in FACS
	
acq10:
	
	mov eax, [ ecx ]
 	mov edx, eax
 	
 	and edx, ~ 1
 	bts edx, 1
 	adc edx, 0
 	
 	lock cmpxchg [ ecx ], edx
 	
 	jnz short acq10
 	
 	cmp dl, 0x03
 	
 	sbb eax, eax
 	
 	ret
	
hw_acpi_aml_globallockrelease:

	mov ecx, [ hw_acpi_aml_globallockaddr ]
	
rel10:
	
	mov eax, [ ecx ]
	mov edx, eax
	
	and edx, ~ 0x03
	
	lock cmpxchg [ ecx ], edx
	
	jnz short rel10
	
	and eax, 1
	
	ret

