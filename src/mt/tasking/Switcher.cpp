#include <mt/tasking/Switcher.h>

#include <interrupt/InterruptHandlers.h>

#include <cpputil/Linkage.h>

ASM_LINKAGE void mt_tasking_switchTask ( void ** OldProcessStack, void * NewProcessStack );
ASM_LINKAGE void mt_tasking_switchTaskInitial ();


void MT::Tasking::Switcher :: SwitchTo ( Task :: Task_t * NewTask, Task :: Task_t * OldTask )
{
	
	//Interrupt::InterruptHandlers :: SetCPInterruptKernelStack ( NewTask -> KStack );
	mt_tasking_switchTask ( & OldTask -> KStack, NewTask -> KStack );
	
};
