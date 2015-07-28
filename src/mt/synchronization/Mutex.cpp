#include <mt/synchronization/Mutex.h>
#include <mt/synchronization/Spinlock.h>

#include <mt/tasking/Scheduler.h>

#include <interrupt/IState.h>

#include <hw/cpu/Processor.h>

#include <system/func/Panic.h>

void MT::Synchronization::Mutex :: Acquire ( Mutex_t * Lock )
{
	
	bool ReInt = Interrupt::IState :: ReadAndSetBlock ();
	Spinlock :: SpinAcquire ( & Lock -> MLock );
	
	::HW::CPU::Processor :: CPUInfo * ThisCPU = ::HW::CPU::Processor :: GetCurrent ();
	Tasking::Task :: Task_t * CurrentTask = ThisCPU -> CurrentTask;
	
	if ( Lock -> Locked )
	{
		
		if ( Lock -> Owner == CurrentTask )
			KPANIC ( "Mutex acquired twice!" );
		
		Lock -> LastWaiter -> Next = CurrentTask;
		Lock -> LastWaiter = CurrentTask;
		
		CurrentTask -> State = Tasking::Task :: kState_Blocked;
		
		Spinlock :: Release ( & Lock -> MLock );
		
		Tasking::Scheduler :: Preemt ();
		
	}
	else
	{
		
		Lock -> Locked = true;
		
		Lock -> LastWaiter = Lock -> Owner = CurrentTask;
		CurrentTask -> Next = NULL;
		
		Spinlock :: Release ( & Lock -> MLock );
		
	}
	
	Interrupt::IState :: WriteBlock ( ReInt );
	
};

bool MT::Synchronization::Mutex :: TryAcquire ( Mutex_t * Lock )
{
	
	bool Acquired = false;
	
	bool ReInt = Interrupt::IState :: ReadAndSetBlock ();
	Spinlock :: SpinAcquire ( & Lock -> MLock );
	
	if ( Lock -> Locked == false )
	{
		
		::HW::CPU::Processor :: CPUInfo * ThisCPU = ::HW::CPU::Processor :: GetCurrent ();
		Tasking::Task :: Task_t * CurrentTask = ThisCPU -> CurrentTask;
		
		Lock -> Locked = true;
		
		Lock -> LastWaiter = Lock -> Owner = CurrentTask;
		CurrentTask -> Next = NULL;
		
		Acquired = true;
		
	}
	
	Spinlock :: Release ( & Lock -> MLock );
	Interrupt::IState :: WriteBlock ( ReInt );
	
	return Acquired;
	
};
			
void MT::Synchronization::Mutex :: Release ( Mutex_t * Lock )
{
	
	bool ReInt = Interrupt::IState :: ReadAndSetBlock ();
	Spinlock :: SpinAcquire ( & Lock -> MLock );
	
	if ( Lock -> Locked == false )
		KPANIC ( "Released an unheld mutex!" );
	
	::HW::CPU::Processor :: CPUInfo * ThisCPU = ::HW::CPU::Processor :: GetCurrent ();
	Tasking::Task :: Task_t * CurrentTask = ThisCPU -> CurrentTask;
	
	if ( Lock -> Owner != CurrentTask )
		KPANIC ( "Task released a mutex which it didn't own!" );
	
	Lock -> Owner = Lock -> Owner -> Next;
	
	if ( Lock -> Owner != NULL )
	{
		
		Lock -> Owner -> State = Tasking::Task :: kState_Runnable;
		
		Tasking::Scheduler :: AddTask ( Lock -> Owner );
		
	}
	else
		Lock -> Locked = false;
	
	Spinlock :: Release ( & Lock -> MLock );
	Interrupt::IState :: WriteBlock ( ReInt );
	
};

void MT::Synchronization::Mutex :: ReleaseFromWrongThread ( Mutex_t * Lock )
{
	
	bool ReInt = Interrupt::IState :: ReadAndSetBlock ();
	Spinlock :: SpinAcquire ( & Lock -> MLock );
	
	if ( Lock -> Locked == false )
		KPANIC ( "Released an unheld mutex!" );
	
	Lock -> Owner = Lock -> Owner -> Next;
	
	if ( Lock -> Owner != NULL )
	{
		
		Lock -> Owner -> State = Tasking::Task :: kState_Runnable;
		
		Tasking::Scheduler :: AddTask ( Lock -> Owner );
		
	}
	else
		Lock -> Locked = false;
	
	Spinlock :: Release ( & Lock -> MLock );
	Interrupt::IState :: WriteBlock ( ReInt );
	
};