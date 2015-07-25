#include <mm/KMalloc.h>
#include <mm/LibAlloc.h>
#include <mm/PMalloc.h>

#include <system/func/kprintf.h>

#include <mt/synchronization/Spinlock.h>
#include <mt/synchronization/Mutex.h>

#include <cpputil/Unused.h>

#include <interrupt/IState.h>

#include <stddef.h>
#include <stdint.h>

MT::Synchronization::Spinlock :: Spinlock_t liballoc_spinlock = MT::Synchronization::Spinlock :: Initializer ();
MT::Synchronization::Mutex :: Mutex_t liballoc_mutex = MT::Synchronization::Mutex :: Initializer ();

bool liballoc_lockmode = false;

bool liballoc_reint = false;

void liballoc_switchlockmode ()
{
	
	liballoc_reint = Interrupt::IState :: ReadAndSetBlock ();
	MT::Synchronization::Spinlock :: SpinAcquire ( & liballoc_spinlock );
	Interrupt::IState :: WriteBlock ( liballoc_reint );
	
	liballoc_lockmode = true;
	
};

int liballoc_lock ()
{
	
	if ( liballoc_lockmode )
		MT::Synchronization::Mutex :: Acquire ( & liballoc_mutex );
	else
	{
		
		liballoc_reint = Interrupt::IState :: ReadAndSetBlock ();
		MT::Synchronization::Spinlock :: SpinAcquire ( & liballoc_spinlock );
		
	}
	
	return 0;
	
};

int liballoc_unlock ()
{
	
	if ( liballoc_lockmode )
		MT::Synchronization::Mutex :: Release ( & liballoc_mutex );
	else
	{
		
		MT::Synchronization::Spinlock :: Release ( & liballoc_spinlock );
		Interrupt::IState :: WriteBlock ( liballoc_reint );
		
	}
	
	return 0;
	
};

void * liballoc_alloc ( size_t Pages )
{
	
	void * Addr = mm_pmalloc ( static_cast <uint32_t> ( Pages ) );
	
	return Addr;
	
};

int liballoc_free ( void * Address, size_t Length )
{
	
	UNUSED ( Length );
	
	mm_pfree ( Address );
	
	return 0;
	
};

#include "LibAlloc.c"
