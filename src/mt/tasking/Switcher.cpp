#include <mt/tasking/Switcher.h>

#include <interrupt/InterruptHandlers.h>

#include <cpputil/Linkage.h>

ASM_LINKAGE void mt_tasking_switchTask ( void * OldProcessStack, void * NewProcessStack );
ASM_LINKAGE void mt_tasking_switchTaskInitial ();

void MT::Tasking::Switcher :: Init ()
{
	
	
	
};

void MT::Tasking::Switcher :: SetupInitISRReturnStack ( Task :: Task_t * EntryTask, void * KStack, uint32_t StackSize )
{
	
	KStack = reinterpret_cast <void *> ( reinterpret_cast <uint32_t> ( KStack ) + StackSize - sizeof ( Interrupt::InterruptHandlers :: ISRFrame * ) );
	
	EntryTask -> KStack = KStack;
	EntryTask -> PState = reinterpret_cast <Interrupt::InterruptHandlers::ISRFrame *> ( KStack );
	
	StackPush ( & EntryTask -> KStack, reinterpret_cast <uint32_t> ( & mt_tasking_switchTaskInitial ) );
	
};

void MT::Tasking::Switcher :: StackPush ( void ** Stack, uint32_t Value )
{
	
	* Stack = reinterpret_cast <void *> ( reinterpret_cast <uint32_t> ( Stack ) - 4 );
	* reinterpret_cast <uint32_t *> ( * Stack ) = Value;
	
};

uint32_t MT::Tasking::Switcher :: StackPop ( void ** Stack )
{
	
	uint32_t Temp = * reinterpret_cast <uint32_t *> ( * Stack );
	* Stack = reinterpret_cast <void *> ( reinterpret_cast <uint32_t> ( Stack ) + 4 );
	
	return Temp;
	
};

uint32_t MT::Tasking::Switcher :: StackPeek ( void ** Stack )
{
	
	return * reinterpret_cast <uint32_t *> ( * Stack );
	
};
