#include <mt/tasking/Reaper.h>
#include <mt/tasking/Scheduler.h>

#include <cpputil/Linkage.h>

#include <system/func/kprintf.h>

MT::Tasking::Task :: Task_t * MT::Tasking::Reaper :: ReaperTask = NULL;

MT::Synchronization::Spinlock :: Spinlock_t MT::Tasking::Reaper :: TaskLock = MT::Synchronization::Spinlock :: Initializer ();

ASM_LINKAGE void mt_tasking_reaperEntry ();

void MT::Tasking::Reaper :: Init ()
{
	
	ReaperTask = Task :: CreateKernelTask ( "Task Reaper", reinterpret_cast <void *> ( & mt_tasking_reaperEntry ), 0x1000, MT::Tasking::Task::kPriority_RealTime_Max, MT::Tasking::Task::kPriority_RealTime_Max );
	
	ReaperTask -> Flags |= Task :: kFlag_Suspended;
	
};
			
void MT::Tasking::Reaper :: Signal ()
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & TaskLock );
	
	if ( ( ReaperTask -> Flags & Task :: kFlag_Suspended ) != 0 )
	{
		
		ReaperTask -> State = Task :: kState_Runnable;
		ReaperTask -> Flags &= ~ Task :: kFlag_Suspended;
		Scheduler :: AddTaskInternal ( ReaperTask );
		
	}
	
	MT::Synchronization::Spinlock :: Release ( & TaskLock );
	
};

void MT::Tasking::Reaper :: Run ()
{
	
	while ( true )
	{
		
		system_func_kprintf ( "Reaper running!\n" );
		
		Task :: Task_t * ToKill = Scheduler :: GetNextDeadTask ();
		
		while ( ToKill != NULL )
		{
			
			if ( ToKill -> Flags & Task :: kFlag_Kernel )
			{
				
				system_func_kprintf ( "Killing kernel task: %s\n", ToKill -> Name );
				
				Task :: DestroyKernelTask ( ToKill );
				
			}
			else
			{
				
				
				
			}
			
			ToKill = Scheduler :: GetNextDeadTask ();
			
		}
		
		system_func_kprintf ( "Reaper suspending!\n" );
		
		Scheduler :: SuspendCurrentTask ();
		
	};
	
};

void mt_tasking_reaperInit ()
{
	
	MT::Tasking::Reaper :: Run ();
	
};
