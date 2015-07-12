#include <hw/cpu/Exceptions.h>

#include <system/func/Panic.h>
#include <system/func/KPrintF.h>

void hw_cpu_exceptionDivideByZero ( Interrupt::InterruptHandlers :: ISRFrame * Frame )
{
	
	KPANIC ( "Divide by zero! \n\n%r", Frame );
	
};

void hw_cpu_exceptionDebug ( Interrupt::InterruptHandlers :: ISRFrame * Frame )
{
	
	system_func_kprintf ( "Debug! \n\n%r", Frame );
	
};

void hw_cpu_exceptionInvalidOpcode ( Interrupt::InterruptHandlers :: ISRFrame * Frame )
{
	
	KPANIC ( "Invalid opcode! \n\nOpcode [%h]: %h\n\n%r", Frame -> EIP, * reinterpret_cast <uint32_t *> ( Frame -> EIP ), Frame );
	
};
