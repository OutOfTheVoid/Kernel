#include <interrupt/IState.h>

#include <hw/cpu/Interrupt.h>
#include <hw/cpu/Processor.h>
#include <hw/cpu/EFlags.h>

uint32_t Interrupt::IState :: BSPCount;

void Interrupt::IState :: Init ()
{
	
	HW::CPU::Processor :: GetCurrent () -> CurrentCLICount = 0;
	
};

void Interrupt::IState :: IncrementBlock ()
{
	
	hw_cpu_cli ();
	
	HW::CPU::Processor :: CPUInfo * CPUI = HW::CPU::Processor :: GetCurrent ();
	
	CPUI -> CurrentCLICount ++;
	
};

void Interrupt::IState :: DecrementBlock ()
{
	
	HW::CPU::Processor :: CPUInfo * CPUI = HW::CPU::Processor :: GetCurrent ();
	
	if ( CPUI -> CurrentCLICount == 0 )
		return;
	
	CPUI -> CurrentCLICount --;
	
	if ( CPUI -> CurrentCLICount == 0 )
		hw_cpu_sti ();
	
};

uint32_t Interrupt::IState :: GetBlockCount ()
{
	
	HW::CPU::Processor :: CPUInfo * CPUI = HW::CPU::Processor :: GetCurrent ();
	
	return CPUI -> CurrentCLICount;
	
};

bool Interrupt::IState :: ReadAndSetBlock ()
{
	
	uint32_t State = hw_cpu_readEFlags ();
	
	hw_cpu_cli ();
	
	return ( State & HW_CPU_EFLAGS_INTERRUPT ) != 0;
	
};

void Interrupt::IState :: WriteBlock ( bool UnBlock )
{
	
	if ( UnBlock )
		hw_cpu_sti ();
	
};
