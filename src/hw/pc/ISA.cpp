#include <hw/pc/ISA.h>

#include <system/func/Panic.h>

void HW::PC::ISA :: SetIRQHandler ( uint8_t IRQ, void ( * Handler ) ( Interrupt::InterruptHandlers :: ISRFrame * ) )
{
	
	if ( IRQ > 0x0F )
		KPANIC ( "Attempt to install out of range ISA IRQ Handler!" );
	
	Interrupt::InterruptHandlers :: SetInterruptHandler ( kIRQ_InterruptBase + IRQ, Handler );
	
};
