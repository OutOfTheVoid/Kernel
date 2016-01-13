#include <mt/tasking/Switcher.h>

#include <interrupt/InterruptHandlers.h>

#include <cpputil/Linkage.h>

#include <hw/cpu/Math.h>

ASM_LINKAGE void mt_tasking_switchTask ( void ** OldProcessStack, void * NewProcessStack );
ASM_LINKAGE void mt_tasking_switchTaskInitial ();

void MT::Tasking::Switcher :: SwitchTo ( Task :: Task_t * NewTask, Task :: Task_t * OldTask )
{
	
	if ( OldTask -> Flags & Task :: kFlag_Math )
	{
		
		if ( ::HW::CPU::Math :: GetTest () )
		{
			
			::HW::CPU::Math :: SaveState ( & OldTask -> Math );
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
			::HW::CPU::Math :: LoadState ( & NewTask -> Math );
		
	}
	
	mt_tasking_switchTask ( & OldTask -> KStack, NewTask -> KStack );
	
};
