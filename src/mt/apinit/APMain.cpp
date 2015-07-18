#include <mt/apinit/APMain.h>
#include <mt/synchronization/Spinlock.h>

#include <system/func/KPrintF.h>

#include <hw/cpu/Processor.h>

#include <interrupt/Interrupt.h>

#include <mt/tasking/Scheduler.h>

void mt_apinit_apmain ()
{
	
	HW::CPU::Processor :: CPUInfo * ThisCPU = HW::CPU::Processor :: GetCurrent ();
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & ThisCPU -> Lock );
	
	ThisCPU -> Flags &= ~ ( HW::CPU::Processor :: kCPUFlag_RealMode | HW::CPU::Processor :: kCPUFlag_StartingUp );
	ThisCPU -> Flags |= HW::CPU::Processor :: kCPUFlag_ProtectedMode | HW::CPU::Processor :: kCPUFlag_Wait;
	
	MT::Synchronization::Spinlock :: Release ( & ThisCPU -> Lock );
	
	bool Enter = false;
	
	while ( ! Enter )
	{
		
		MT::Synchronization::Spinlock :: SpinAcquire ( & ThisCPU -> Lock );
		
		Enter = ( ThisCPU -> Flags & HW::CPU::Processor :: kCPUFlag_Wait ) == 0;
		
		MT::Synchronization::Spinlock :: Release ( & ThisCPU -> Lock );
		
	}
	
	system_func_kprintf ( "Processor %i entered! Beginning tasking...\n", ThisCPU -> Index );
	
	Interrupt :: APInit ();
	
	system_func_kprintf ( "Processor %i enabled interrupts...\n", ThisCPU -> Index );
	
	MT::Tasking::Scheduler :: PInit ();
	
	system_func_kprintf ( "Processor %i started scheduler...\n", ThisCPU -> Index );
	
	__asm__ volatile ( "int 0x20" );
	
	system_func_kprintf ( "Processor %i scheduled, killing init task...\n", ThisCPU -> Index );
	
	MT::Tasking::Scheduler :: KillCurrentTask ();
	
};
