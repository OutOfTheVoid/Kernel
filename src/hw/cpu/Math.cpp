#include <hw/cpu/Math.h>

void HW::CPU::Math :: SaveState ( MathState * State )
{
	
	__asm__ volatile ( "fxsave" : : "m" ( State ) );
	
};

void HW::CPU::Math :: LoadState ( MathState * State )
{
	
	__asm__ volatile ( "fxstore" : : "m" ( State ) );
	
};
