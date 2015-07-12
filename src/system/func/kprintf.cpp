#include <system/func/KPrintF.h>

#include <mt/synchronization/Spinlock.h>

#include <interrupt/IState.h>

void ( * system_func_kprintfOutputFunction ) ( const char *, va_list Args );

MT::Synchronization::Spinlock :: Spinlock_t system_func_kprintf_PrintLock = MT::Synchronization::Spinlock :: Initializer ();

C_LINKAGE void system_func_kprintfOutput ( void ( * Output ) ( const char *, va_list Args ) )
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & system_func_kprintf_PrintLock );
	bool IntBlock = Interrupt::IState :: ReadAndSetBlock ();
	
	system_func_kprintfOutputFunction = Output;
	
	Interrupt::IState :: WriteBlock ( IntBlock );
	MT::Synchronization::Spinlock :: Release ( & system_func_kprintf_PrintLock );
	
}

C_LINKAGE void system_func_kprintf ( const char * FormatString, ... )
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & system_func_kprintf_PrintLock );
	bool IntBlock = Interrupt::IState :: ReadAndSetBlock ();
	
	va_list Args;

	va_start ( Args, FormatString );
	( * system_func_kprintfOutputFunction ) ( FormatString, Args );
	va_end ( Args );
	
	Interrupt::IState :: WriteBlock ( IntBlock );
	MT::Synchronization::Spinlock :: Release ( & system_func_kprintf_PrintLock );
	
};
