#include <interrupt/Interrupt.h>
#include <interrupt/InterruptHandlers.h>
#include <interrupt/PIC.h>
#include <interrupt/APIC.h>
#include <interrupt/IOAPIC.h>

#include <hw/cpu/Exceptions.h>

#include <hw/pc/ISA.h>

#include <hw/acpi/ACPI.h>

#include <system/func/kprintf.h>

#include <KernelDef.h>

void Interrupt :: APICInitEarly ()
{
	
	PIC :: Init ();
	APIC :: Init ();
	
};

void Interrupt :: Init ()
{
	
	InterruptHandlers :: Init ();
	
	InterruptHandlers :: SetInterruptHandler ( 0x00, & hw_cpu_exceptionDivideByZero );
	InterruptHandlers :: SetInterruptHandler ( 0x01, & hw_cpu_exceptionDebug );
	InterruptHandlers :: SetInterruptHandler ( 0x02, & hw_cpu_exceptionNMI );
	InterruptHandlers :: SetInterruptHandler ( 0x03, & hw_cpu_exceptionBreakpoint );
	InterruptHandlers :: SetInterruptHandler ( 0x04, & hw_cpu_exceptionOverflow );
	InterruptHandlers :: SetInterruptHandler ( 0x05, & hw_cpu_exceptionBoundRangeExceded );
	InterruptHandlers :: SetInterruptHandler ( 0x06, & hw_cpu_exceptionInvalidOpcode );
	InterruptHandlers :: SetInterruptHandler ( 0x07, & hw_cpu_exceptionDeviceUnavailible );
	InterruptHandlers :: SetInterruptHandler ( 0x08, & hw_cpu_exceptionDoubleFault );
	
	InterruptHandlers :: SetInterruptHandler ( 0x0A, & hw_cpu_exceptionInvalidTSS );
	InterruptHandlers :: SetInterruptHandler ( 0x0B, & hw_cpu_exceptionNoSegmentPresent );
	InterruptHandlers :: SetInterruptHandler ( 0x0C, & hw_cpu_exceptionSSFault );
	InterruptHandlers :: SetInterruptHandler ( 0x0D, & hw_cpu_exceptionGeneralProtection );
	InterruptHandlers :: SetInterruptHandler ( 0x0E, & hw_cpu_exceptionPageFault );
	
	InterruptHandlers :: SetInterruptHandler ( 0x10, & hw_cpu_exceptionFloatingPoint );
	InterruptHandlers :: SetInterruptHandler ( 0x11, & hw_cpu_exceptionAlignmentCheck );
	InterruptHandlers :: SetInterruptHandler ( 0x12, & hw_cpu_exceptionMachineCheck );
	InterruptHandlers :: SetInterruptHandler ( 0x13, & hw_cpu_exceptionSIMDException );
	
	InterruptHandlers :: SetInterruptHandler ( 0x1E, & hw_cpu_exceptionSecurity );
	
	IOAPIC :: Init ();
	
	HW::PC::ISA :: InitIOInterrupts ();
	
	uint32_t ACPIStatus;
	HW::ACPI :: InitInterrupts ( & ACPIStatus );
	
};

void Interrupt :: APInit ()
{
	
	InterruptHandlers :: APInit ();
	APIC :: APInit ();
	
};

