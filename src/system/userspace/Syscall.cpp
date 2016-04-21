#include <system/userspace/Syscall.h>

#include <hw/cpu/Processor.h>

#include <mt/tasking/Scheduler.h>
#include <interrupt/APIC.h>

#include <cpputil/Unused.h>

#include <mt/userspace/MTSyscalls.h>

void ( * System::UserSpace::Syscall :: Handlers [ System::UserSpace::Syscall :: kNumSyscalls ] ) ( Interrupt::InterruptHandlers :: ISRFrame * Frame, volatile MT::Tasking :: Task * CurrentTask );

void System::UserSpace::Syscall :: Init ()
{
	
	for ( uint32_t I = 0; I < kNumSyscalls; I ++ )
		Handlers [ I ] = & DummySyscall;
	
	Handlers [ kSyscall_Exit ] = & MT::UserSpace::MTSyscalls :: TaskExit;
	
	Interrupt::InterruptHandlers :: SetInterruptHandler ( kSysCallInterrupt_Vector, CallInterruptHandler );
	
};

void System::UserSpace::Syscall :: CallInterruptHandler ( Interrupt::InterruptHandlers :: ISRFrame * Frame )
{
	
	HW::CPU::Processor :: CPUInfo * ThisCPU = HW::CPU::Processor :: GetCurrent ();
	
	uint32_t CallNumber = Frame -> EAX;
	
	if ( CallNumber >= kNumSyscalls )
	{
		
		Frame -> EAX = 0xFFFFFFFF;
		
		Interrupt::APIC :: EndOfInterrupt ();
		
		return;
		
	}
	
	return ( Handlers [ CallNumber ] ) ( Frame, ThisCPU -> CurrentTask );
	
};

void System::UserSpace::Syscall :: DummySyscall ( Interrupt::InterruptHandlers :: ISRFrame * Frame, volatile MT::Tasking :: Task * CurrentTask )
{
	
	UNUSED ( CurrentTask );
	
	system_func_kprintf ( "Dummy system call from Task: %s\n", CurrentTask -> Name );
	
	Frame -> EAX = 0xFFFFFFFF;
		
	Interrupt::APIC :: EndOfInterrupt ();
	
};
