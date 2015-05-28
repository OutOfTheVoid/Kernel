#include <interrupt/IState.h>
#include <hw/cpu/interrupt.h>

uint32_t Interrupt::IState :: Count;

void Interrupt::IState :: Init ()
{
	
	Count = 0;
	
};

void Interrupt::IState :: IncrementBlock ()
{
	
	if ( Count == 0 )
		hw_cpu_cli ();
	
	Count ++;
	
};

void Interrupt::IState :: DecrementBlock ()
{
	
	if ( Count == 0 )
		return;
	
	Count --;
	
	if ( Count == 0 )
		hw_cpu_sei ();
	
};
