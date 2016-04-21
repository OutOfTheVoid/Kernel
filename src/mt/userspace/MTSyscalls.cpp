#include <mt/userspace/MTSyscalls.h>

#include <mt/tasking/Scheduler.h>

#include <interrupt/APIC.h>

#include <hw/cpu/Interrupt.h>

#include <cpputil/Unused.h>

void MT::UserSpace::MTSyscalls :: TaskExit ( Interrupt::InterruptHandlers :: ISRFrame * Frame, volatile MT::Tasking :: Task * CurrentTask )
{
	
	UNUSED ( Frame );
	//UNUSED ( CurrentTask );
	
	Interrupt::APIC :: EndOfInterrupt ();
	hw_cpu_sti ();
	
	system_func_kprintf ( "EXIT: %s\n", CurrentTask -> Name );
	
	MT::Tasking::Scheduler :: KillCurrentTask ();
	
};
