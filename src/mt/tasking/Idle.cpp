#include <mt/tasking/Idle.h>

#include <hw/cpu/Hang.h>
#include <hw/cpu/Interrupt.h>

#include <system/func/KPrintF.h>

ASM_LINKAGE void mt_tasking_idle ();

void mt_tasking_idle ()
{
	
	while ( true ) hw_cpu_hang ();
	
};

