#include <mt/synchronization/Mutex.h>
#include <mt/synchronization/Spinlock.h>

#include <mt/tasking/Scheduler.h>

#include <interrupt/IState.h>

#include <hw/cpu/Processor.h>

#include <system/func/Panic.h>

void MT::Synchronization::Mutex :: Acquire ( Mutex_t * Lock )
{
	
	Spinlock :: SpinAcquire ( & Lock -> MLock );
	
	volatile Tasking::Task * CurrentTask = ::HW::CPU::Processor :: GetCurrent () -> CurrentTask;
	
	if ( Lock -> Locked )
	{
		
		if ( Lock -> Owner == CurrentTask )
			KPANIC ( "Mutex acquired twice!" );
		
		if ( Lock -> FirstWaiter == NULL )
		{
			
			Lock -> FirstWaiter = CurrentTask;
			Lock -> LastWaiter = CurrentTask;
			
		}
		else
		{
			
			Lock -> LastWaiter -> Next = CurrentTask;
			Lock -> LastWaiter = CurrentTask;
			
		}
		
		CurrentTask -> Next = NULL;
		CurrentTask -> State = Tasking::Task :: kState_Blocked;
		
		MT::Tasking::Scheduler :: Preemt ( & Lock -> MLock );
		
	}
	else
	{
		
		Lock -> Locked = true;
		Lock -> Owner = CurrentTask;
		
		Spinlock :: Release ( & Lock -> MLock );
		
	}
	
};

bool MT::Synchronization::Mutex :: TryAcquire ( Mutex_t * Lock )
{
	
	Spinlock :: SpinAcquire ( & Lock -> MLock );
	
	volatile Tasking::Task * CurrentTask = ::HW::CPU::Processor :: GetCurrent () -> CurrentTask;
	
	if ( Lock -> Locked )
	{
		
		Spinlock :: Release ( & Lock -> MLock );
		
		return false;
		
	}
	
	Lock -> Locked = true;
	Lock -> Owner = CurrentTask;
	
	Spinlock :: Release ( & Lock -> MLock );
	
	return true;
	
};
			
void MT::Synchronization::Mutex :: Release ( Mutex_t * Lock )
{
	
	Spinlock :: SpinAcquire ( & Lock -> MLock );
	
	volatile Tasking::Task * CurrentTask = ::HW::CPU::Processor :: GetCurrent () -> CurrentTask;
	
	if ( ! Lock -> Locked )
		KPANIC ( "Unheld Mutex released!" );
	
	if ( Lock -> Owner != CurrentTask )
		KPANIC ( "Mutex released from wrong thread!" );
	
	if ( Lock -> FirstWaiter != NULL )
	{
		
		volatile Tasking::Task * NextOwner = Lock -> FirstWaiter;
		Lock -> FirstWaiter = NextOwner -> Next;
		
		if ( Lock -> FirstWaiter == NULL )
			Lock -> LastWaiter = NULL;
		
		NextOwner -> State = Tasking::Task :: kState_Runnable;
		Lock -> Owner = NextOwner;
		
		Tasking::Scheduler :: AddTask ( const_cast <Tasking::Task *> ( NextOwner ) );
		
	}
	else
	{
		
		Lock -> Owner = NULL;
		Lock -> Locked = false;
		
	}
	
	Spinlock :: Release ( & Lock -> MLock );
	
};

void MT::Synchronization::Mutex :: ReleaseFromWrongThread ( Mutex_t * Lock )
{
	
	Spinlock :: SpinAcquire ( & Lock -> MLock );
	
	if ( ! Lock -> Locked )
		KPANIC ( "Unheld Mutex released!" );
	
	if ( Lock -> FirstWaiter != NULL )
	{
		
		volatile Tasking::Task * NextOwner = Lock -> FirstWaiter;
		Lock -> FirstWaiter = NextOwner -> Next;
		
		if ( Lock -> FirstWaiter == NULL )
			Lock -> LastWaiter = NULL;
		
		NextOwner -> State = Tasking::Task :: kState_Runnable;
		Lock -> Owner = NextOwner;
		
		Tasking::Scheduler :: AddTask ( const_cast <Tasking::Task *> ( NextOwner ) );
		
	}
	else
	{
		
		Lock -> Owner = NULL;
		Lock -> Locked = false;
		
	}
	
	Spinlock :: Release ( & Lock -> MLock );
	
};