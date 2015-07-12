#include <interrupt/Interrupt.h>
#include <interrupt/InterruptHandlers.h>
#include <interrupt/PIC.h>
#include <interrupt/APIC.h>

#include <hw/cpu/Exceptions.h>

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
	
	InterruptHandlers :: SetInterruptHandler ( 0, & hw_cpu_exceptionDivideByZero );
	InterruptHandlers :: SetInterruptHandler ( 1, & hw_cpu_exceptionDebug );
	InterruptHandlers :: SetInterruptHandler ( 6, & hw_cpu_exceptionInvalidOpcode );
	
};

void Interrupt :: APInit ()
{
	
	
	
};

