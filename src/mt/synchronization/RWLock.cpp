#include <mt/synchronization/RWLock.h>
#include <mt/tasking/Scheduler.h>

#include <hw/cpu/Atomic.h>

void MT::Synchronization::RWLock :: ReadAcquire ( RWLock_t * Lock, bool Spin )
{
	
	bool One = ::HW::CPU::Atomic :: IncrementTestOne ( & Lock -> ReadersAtomic );
	
	if ( One )
	{
		
		MT::Synchronization::Mutex :: Acquire ( & Lock -> WLock );
		Lock -> ReaderAcquired = true;
		
	}
	else
	{
		
		if ( ! Spin )
		{
			
			while ( ! Lock -> ReaderAcquired )	
				MT::Tasking::Scheduler :: YieldCurrentTask ();
		
		}
		else
			while ( ! Lock -> ReaderAcquired );
				//__asm__ volatile ( "pause" );
		
	}
	
};

void MT::Synchronization::RWLock :: ReadRelease ( RWLock_t * Lock )
{
	
	bool Zero = ::HW::CPU::Atomic :: DecrementTestZero ( & Lock -> ReadersAtomic );
	
	if ( Zero )
		MT::Synchronization::Mutex :: Release ( & Lock -> WLock );
	
	Lock -> ReaderAcquired = false;
	
};

void MT::Synchronization::RWLock :: WriteAcquire ( RWLock_t * Lock )
{
	
	MT::Synchronization::Mutex :: Acquire ( & Lock -> WLock );
	
};

void MT::Synchronization::RWLock :: WriteRelease ( RWLock_t * Lock )
{
	
	MT::Synchronization::Mutex :: Release ( & Lock -> WLock );
	
};
