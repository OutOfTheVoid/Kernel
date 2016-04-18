#include <mt/tasking/Switcher.h>

#include <interrupt/InterruptHandlers.h>

#include <cpputil/Linkage.h>

#include <hw/cpu/Math.h>

ASM_LINKAGE void mt_tasking_switchTask ( void * volatile * OldProcessStack, volatile void * NewProcessStack );
ASM_LINKAGE void mt_tasking_switchTaskInitial ();

extern void * mt_tasking_switcher_TaskListLockRef;

void MT::Tasking::Switcher :: Init ( Synchronization::Spinlock :: Spinlock_t * TaskTableLock )
{
	
	mt_tasking_switcher_TaskListLockRef = reinterpret_cast <void *> ( & TaskTableLock -> Lock );
	
};

void MT::Tasking::Switcher :: SwitchTo ( volatile Task :: Task_t * NewTask, volatile Task :: Task_t * OldTask )
{
	
	if ( OldTask -> Flags & Task :: kFlag_Math )
	{
		
		if ( ::HW::CPU::Math :: GetTest () )
		{
			
			::HW::CPU::Math :: SaveState ( const_cast < ::HW::CPU::Math :: MathState *> ( & OldTask -> Math ) );
			::HW::CPU::Math :: ClearTest ();
			
		}
		
	}
	
	if ( NewTask -> Flags & Task :: kFlag_Math )
	{
		
		if ( ! ( ( NewTask -> Flags & Task :: kFlag_MathInit ) == 0 ) )
		{
			
			::HW::CPU::Math :: InitState ();
			
			NewTask -> Flags |= Task :: kFlag_MathInit;
			
		}
		else
			::HW::CPU::Math :: LoadState ( const_cast < ::HW::CPU::Math :: MathState *> ( & NewTask -> Math ) );
		
	}
	
	mt_tasking_switchTask ( & OldTask -> KStack, NewTask -> KStack );
	
};
