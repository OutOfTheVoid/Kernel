#include <mt/tasking/Idle.h>

#include <hw/cpu/Hang.h>
#include <hw/cpu/Interrupt.h>

#include <system/func/KPrintF.h>

ASM_LINKAGE void mt_tasking_idle ();

void mt_tasking_idle ()
{
	
	//system_func_kprintf ( "\n* IDLE TASK: Hello world! *\n" );
	
	hw_cpu_cli ();
	hw_cpu_hang ();
	
};

