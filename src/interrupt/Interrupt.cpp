#include <interrupt/Interrupt.h>
#include <interrupt/InterruptHandlers.h>
#include <interrupt/PIC.h>
#include <interrupt/APIC.h>

#include <system/func/kprintf.h>

#include <KernelDef.h>

void Interrupt :: APICInitEarly ()
{
	
	PIC :: Init ();
	APIC :: Init ();
	
};

void Interrupt :: Init ()
{	
};

void Interrupt :: APInit ()
{
	
	
	
};

