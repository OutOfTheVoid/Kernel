#include <hw/cpu/Exceptions.h>

#include <system/func/Panic.h>
#include <system/func/KPrintF.h>

#include <mt/exception/MPException.h>

#include <mm/paging/Invalidation.h>

#include <hw/cpu/Hang.h>

void hw_cpu_exceptionDivideByZero ( Interrupt::InterruptHandlers :: ISRFrame * Frame )
{
	
	KPANIC ( "Divide by zero! \n\n%r", Frame );
	
};

void hw_cpu_exceptionDebug ( Interrupt::InterruptHandlers :: ISRFrame * Frame )
{
	
	KPANIC ( "Debug! \n\n%r", Frame );
	
};

void hw_cpu_exceptionNMI ( Interrupt::InterruptHandlers :: ISRFrame * Frame )
{
	
	if ( ! MT::Exception::MPException :: HasTriggered () )
		KPANIC ( "NMI!\n\n%r", Frame );
	else
		hw_cpu_hang ();
	
};

void hw_cpu_exceptionBreakpoint ( Interrupt::InterruptHandlers :: ISRFrame * Frame )
{
	
	( void ) Frame;
	
};

void hw_cpu_exceptionOverflow ( Interrupt::InterruptHandlers :: ISRFrame * Frame )
{
	
	KPANIC ( "Overflow!\n\n%r", Frame );
	
};

void hw_cpu_exceptionBoundRangeExceded ( Interrupt::InterruptHandlers :: ISRFrame * Frame )
{
	
	KPANIC ( "Out of bounds!\n\n%r", Frame );
	
};

void hw_cpu_exceptionInvalidOpcode ( Interrupt::InterruptHandlers :: ISRFrame * Frame )
{
	
	KPANIC ( "Invalid opcode!\n\nOpcode [%h]: %h\n\n%r", Frame -> EIP, * reinterpret_cast <uint32_t *> ( Frame -> EIP ), Frame );
	
};

void hw_cpu_exceptionDeviceUnavailible ( Interrupt::InterruptHandlers :: ISRFrame * Frame )
{
	
	KPANIC ( "Device not availible!\n\n%r", Frame );
	
};

void hw_cpu_exceptionDoubleFault ( Interrupt::InterruptHandlers :: ISRFrame * Frame )
{
	
	KPANIC ( "Double fault!\n\n%r", Frame );
	
};

void hw_cpu_exceptionInvalidTSS ( Interrupt::InterruptHandlers :: ISRFrame * Frame )
{
	
	KPANIC ( "Invalid TSS!\n\n%r", Frame );
	
};

void hw_cpu_exceptionNoSegmentPresent ( Interrupt::InterruptHandlers :: ISRFrame * Frame )
{
	
	KPANIC ( "Invalid segment!\n\n%r", Frame );
	
};

void hw_cpu_exceptionSSFault ( Interrupt::InterruptHandlers :: ISRFrame * Frame )
{
	
	KPANIC ( "Stack segment fault!\n\n%r", Frame );
	
};

void hw_cpu_exceptionGeneralProtection ( Interrupt::InterruptHandlers :: ISRFrame * Frame )
{
	
	KPANIC ( "General protection!\n\n%r", Frame );
	
};

void hw_cpu_exceptionPageFault ( Interrupt::InterruptHandlers :: ISRFrame * Frame )
{
	
	if ( MM::Paging::Invalidation :: PageFault ( Frame -> ErrorCode ) )
	{
		uint32_t CR2 = MM::Paging::Invalidation :: GetCR2 ();
		KPANIC ( "Page fault!\n\n%r\n\nCR2: %h", Frame, CR2 );
	}
	
};

void hw_cpu_exceptionFloatingPoint ( Interrupt::InterruptHandlers :: ISRFrame * Frame )
{
	
	KPANIC ( "FPU fault!\n\n%r", Frame );
	
};

void hw_cpu_exceptionAlignmentCheck ( Interrupt::InterruptHandlers :: ISRFrame * Frame )
{
	
	KPANIC ( "Alignment check!\n\n%r", Frame );
	
};

void hw_cpu_exceptionMachineCheck ( Interrupt::InterruptHandlers :: ISRFrame * Frame )
{
	
	KPANIC ( "Machine check!\n\n%r", Frame );
	
};

void hw_cpu_exceptionSIMDException ( Interrupt::InterruptHandlers :: ISRFrame * Frame )
{
	
	KPANIC ( "SIMD Exception!\n\n%r", Frame );
	
};

void hw_cpu_exceptionSecurity ( Interrupt::InterruptHandlers :: ISRFrame * Frame )
{
	
	KPANIC ( "Security exception!\n\n%r", Frame );
	
};
