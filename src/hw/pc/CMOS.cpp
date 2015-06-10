#include <hw/pc/CMOS.h>
#include <hw/cpu/IO.h>

#include <system/func/Panic.h>

void HW::PC::CMOS :: Write ( uint8_t Register, uint8_t Value )
{
	
	if ( Register > 0x80 )
		KPANIC ( "Attempt to write to out of bounds CMOS register!" );
	
	CPU::IO :: Out8 ( kPort_Index, Register );
	CPU::IO :: Out8 ( kPort_Data, Value );
	
};

uint8_t HW::PC::CMOS :: Read ( uint8_t Register )
{
	
	if ( Register > 0x80 )
		KPANIC ( "Attempt to read from out of bounds CMOS register!" );
	
	CPU::IO :: Out8 ( kPort_Index, Register );
	
	return CPU::IO :: In8 ( kPort_Data );
	
};
