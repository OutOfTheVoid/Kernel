#include <interrupt/Interrupt.h>
#include <interrupt/APIC.h>

#include <system/func/kprintf.h>

#include <KernelDef.h>

void Interrupt :: Init ()
{
	
	APIC :: Init ();
	
	
};

