#include <system/userspace/syscall/Syscall.h>

#include <hw/cpu/Processor.h>

#include <mt/tasking/Scheduler.h>

void System::UserSpace::Syscall :: CallHandler ( Interrupt::InterruptHandlers :: ISRFrame * Frame )
{
	
	HW::CPU::Processor :: CPUInfo * ThisCPU = HW::CPU::Processor :: GetCurrent ();
	
	
};
