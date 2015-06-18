#include <mt/apinit/APMain.h>
#include <mt/synchronization/Spinlock.h>

#include <system/func/KPrintF.h>

#include <hw/cpu/Processor.h>

void mt_apinit_apmain ()
{
	
	HW::CPU::Processor :: CPUInfo * ThisCPU = HW::CPU::Processor :: GetCurrent ();
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & ThisCPU -> Lock );
	
	ThisCPU -> Flags &= ~ ( HW::CPU::Processor :: kCPUFlag_RealMode | HW::CPU::Processor :: kCPUFlag_StartingUp );
	ThisCPU -> Flags |= HW::CPU::Processor :: kCPUFlag_ProtectedMode;
	
	MT::Synchronization::Spinlock :: Release ( & ThisCPU -> Lock );
	
	system_func_kprintf ( "Processor started!" );
	
	for (;;);
	
};
