#include <mt/apinit/APMain.h>
#include <mt/synchronization/Spinlock.h>

#include <system/func/KPrintF.h>

#include <hw/cpu/Processor.h>

#include <interrupt/Interrupt.h>

#include <mt/tasking/Scheduler.h>

#include <mm/segmentation/GDT.h>

void mt_apinit_apmain ()
{
	
	HW::CPU::Processor :: CPUInfo * ThisCPU = HW::CPU::Processor :: GetCurrent ();
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & ThisCPU -> Lock );
	
	ThisCPU -> Flags &= ~ ( HW::CPU::Processor :: kCPUFlag_RealMode | HW::CPU::Processor :: kCPUFlag_StartingUp );
	ThisCPU -> Flags |= HW::CPU::Processor :: kCPUFlag_ProtectedMode | HW::CPU::Processor :: kCPUFlag_Wait;
	
	MT::Synchronization::Spinlock :: Release ( & ThisCPU -> Lock );
	
	MM::Segmentation::GDT :: Swap ();
	
	bool Enter = false;
	
	while ( ! Enter )
	{
		
		MT::Synchronization::Spinlock :: SpinAcquire ( & ThisCPU -> Lock );
		
		Enter = ( ThisCPU -> Flags & HW::CPU::Processor :: kCPUFlag_Wait ) == 0;
		
		MT::Synchronization::Spinlock :: Release ( & ThisCPU -> Lock );
		
	}
	
	Interrupt :: APInit ();
	MT::Tasking::Scheduler :: PInit ();
	
	MT::Tasking::Scheduler :: KillCurrentTask ();
	
};
