#include <interrupt/IRQ.h>
#include <interrupt/IState.h>
#include <interrupt/PIC.h>

#include <system/func/panic.h>

#include <cpputil/linkage.h>

ASM_LINKAGE void interrupt_IRQ_CommonIRQHandler ( Interrupt::InterruptHandlers :: IRQFrame * Frame );

void ( * Interrupt::IRQ :: IRQHandlers [ 0x20 ] ) ( InterruptHandlers :: IRQFrame * );

uint8_t Interrupt::IRQ :: MaxIRQ = 0;
Interrupt::IRQ :: IRQExit Interrupt::IRQ :: ExitMode = Interrupt::IRQ :: kIRQExit_None;

MT::Synchronization::Spinlock :: Spinlock_t Interrupt::IRQ :: ListLocks [ 0x20 ];

void Interrupt::IRQ :: Init ()
{
	
	for ( uint32_t I = 0; I < 0x20; I ++ )
	{
		
		IRQHandlers [ I ] = NULL;
		
		ListLocks [ I ] = MT::Synchronization::Spinlock :: Initializer ();
		
	}
	
};

void Interrupt::IRQ :: SetIRQ ( uint8_t IRQNumber, void ( * Handler ) ( InterruptHandlers :: IRQFrame * ) )
{
	
	if ( IRQNumber >= 0x10 )
		return;
	
	bool DoSEI = Interrupt::IState :: ReadAndSetBlock ();
	
	IRQHandlers [ IRQNumber ] = Handler;
	
	Interrupt::IState :: WriteBlock ( DoSEI );
	
};

void Interrupt::IRQ :: CallIRQ ( InterruptHandlers :: IRQFrame * Frame )
{
	
	uint8_t IRQNumber = Frame -> IRQNumber;
	void ( * Handler ) ( InterruptHandlers :: IRQFrame * );
	
	// APIC SPURIOUS INTERRUPT
	if ( IRQNumber == 0xFF )
		return;
	
	if ( IRQNumber > MaxIRQ )
		system_func_panic ( "IRQ out of range called!" );
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & ListLocks [ IRQNumber ] );
	
	Handler = IRQHandlers [ IRQNumber ];
	
	MT::Synchronization::Spinlock :: Release ( & ListLocks [ IRQNumber ] );
	
	if ( Handler != NULL )
		( * Handler ) ( Frame );
	
};

void Interrupt::IRQ :: EndIRQ ( uint8_t Number )
{
	
	switch ( ExitMode )
	{
		
	case kIRQExit_PIC:
		return Interrupt::PIC :: EndOfIRQ ( Number );
		
	case kIRQExit_APIC:
		return Interrupt::APIC :: EndOfInterrupt ( Number );
		
	case kIRQExit_None:
		return;
		
	}
	
};

void interrupt_IRQ_CommonIRQHandler ( Interrupt::InterruptHandlers :: IRQFrame * Frame )
{
	
	Interrupt::IRQ :: CallIRQ ( Frame );
	Interrupt::IRQ :: EndIRQ ( Frame -> IRQNumber );
	
};

