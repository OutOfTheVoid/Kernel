#include <mt/synchronization/RWLock.h>

#include <hw/cpu/Atomic.h>

void MT::Synchronization::RWLock :: ReadAcquire ( RWLock_t * Lock )
{
	
	bool One = ::HW::CPU::Atomic :: IncrementTestOne ( & Lock -> ReadersAtomic );
	
	if ( One )
		MT::Synchronization::Mutex :: Acquire ( & Lock -> WLock );
	
};

void MT::Synchronization::RWLock :: ReadRelease ( RWLock_t * Lock )
{
	
	bool Zero = ::HW::CPU::Atomic :: DecrementTestZero ( & Lock -> ReadersAtomic );
	
	if ( Zero )
		MT::Synchronization::Mutex :: Release ( & Lock -> WLock );
	
};

void MT::Synchronization::RWLock :: WriteAcquire ( RWLock_t * Lock )
{
	
	MT::Synchronization::Mutex :: Acquire ( & Lock -> WLock );
	
};

void MT::Synchronization::RWLock :: WriteRelease ( RWLock_t * Lock )
{
	
	MT::Synchronization::Mutex :: Release ( & Lock -> WLock );
	
};
