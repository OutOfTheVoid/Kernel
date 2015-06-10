#include <interrupt/IState.h>

#include <hw/cpu/interrupt.h>

uint32_t Interrupt::IState :: BSPCount;

void Interrupt::IState :: Init ()
{
	
	BSPCount = 0;
	
};

void Interrupt::IState :: IncrementBlock ()
{
	
	if ( BSPCount == 0 )
		hw_cpu_cli ();
	
	BSPCount ++;
	
};

void Interrupt::IState :: DecrementBlock ()
{
	
	if ( BSPCount == 0 )
		return;
	
	BSPCount --;
	
	if ( BSPCount == 0 )
		hw_cpu_sei ();
	
};

uint32_t Interrupt::IState :: GetBlockCount ()
{
	
	return BSPCount;
	
};
