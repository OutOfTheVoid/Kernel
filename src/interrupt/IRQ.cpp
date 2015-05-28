#include <interrupt/IRQ.h>
#include <interrupt/IState.h>
#include <interrupt/PIC.h>

#include <system/func/panic.h>

#include <cpputil/linkage.h>

C_LINKAGE void interrupt_IRQ_CommonIRQHandler ( Interrupt::IRQ :: IRQFrame * Frame );

void ( * Interrupt::IRQ :: IRQHandlers [ 0x20 ] ) ( IRQFrame * );

uint8_t Interrupt::IRQ :: MaxIRQ = 0;
Interrupt::IRQ :: IRQExit Interrupt::IRQ :: ExitMode = Interrupt::IRQ :: kIRQExit_None;

void Interrupt::IRQ :: Init ()
{
	
	for ( uint32_t i = 0; i < 0x20; i ++ )
		IRQHandlers [ i ] = NULL;
	
};

void Interrupt::IRQ :: SetIRQ ( uint8_t IRQNumber, void ( * Handler ) ( IRQFrame * ) )
{
	
	if ( IRQNumber >= 0x10 )
		return;
	
	Interrupt::IState :: IncrementBlock ();
	
	IRQHandlers [ IRQNumber ] = Handler;
	
	Interrupt::IState :: DecrementBlock ();
	
};

void Interrupt::IRQ :: CallIRQ ( IRQFrame * Frame )
{
	
	uint8_t IRQNumber = Frame -> IRQNumber;
	void ( * Handler ) ( IRQFrame * );
	
	if ( IRQNumber > MaxIRQ )
		system_func_panic ( "IRQ out of range called!" );
	
	Handler = IRQHandlers [ IRQNumber ];
	
	if ( Handler != NULL )
		( * Handler ) ( Frame );
	
};

void Interrupt::IRQ :: EndIRQ ( uint8_t Number )
{
	
	switch ( ExitMode )
	{
		
	case kIRQExit_PIC:
		return Interrupt::PIC :: EndOfIRQ ( Number );
		
	case kIRQExit_None:
		return;
		
	}
	
};

void interrupt_IRQ_CommonIRQHandler ( Interrupt::IRQ :: IRQFrame * Frame )
{
	
	Interrupt::IRQ :: CallIRQ ( Frame );
	Interrupt::IRQ :: EndIRQ ( Frame -> IRQNumber );
	
};

