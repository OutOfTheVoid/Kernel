#include <mt/timing/PWaitMS.h>
#include <mt/synchronization/Spinlock.h>

#include <stddef.h>

void ( * __mt_timing_pwaitmsMechanism ) ( double ) = NULL;
MT::Synchronization::Spinlock :: Spinlock_t mt_timing_pwaitms_WaitLock = MT::Synchronization::Spinlock :: Initializer ();

void mt_timing_pwaitms ( double TimeMS )
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & mt_timing_pwaitms_WaitLock );
	
	( * __mt_timing_pwaitmsMechanism ) ( TimeMS );
	
	MT::Synchronization::Spinlock :: Release ( & mt_timing_pwaitms_WaitLock );
	
};

void mt_timing_pwaitmsMechanism ( void ( * Mechanism ) ( double TimeMS ) )
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & mt_timing_pwaitms_WaitLock );
	
	__mt_timing_pwaitmsMechanism = Mechanism;
	
	MT::Synchronization::Spinlock :: Release ( & mt_timing_pwaitms_WaitLock );
	
};
