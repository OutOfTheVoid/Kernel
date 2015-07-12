#include <mt/tasking/Task.h>
#include <mt/tasking/Scheduler.h>

MT::Tasking::Task :: Task_t * MT::Tasking::Task :: CreateKernelTask ( const char * Name, void * Entry, uint32_t StackSize, uint32_t Priority )
{
	
	Task_t * New = reinterpret_cast <MT::Tasking::Task :: Task_t *> ( mm_kmalloc ( sizeof ( Task :: Task_t ) ) );
	
	if ( New == NULL )
		return NULL;
	
	New -> Name = Name;
	New -> ID = Scheduler :: GetNewTaskID ();
	
	New -> Flags = kFlag_Kernel;
	New -> State = kState_Runnable;
	New -> WaitAttribute = NULL;
	New -> User = 0;
	New -> Privelege = kPrivelege_Exec | kPrivelege_IO;
	New -> Priority = Priority;
	
	New -> KStack = /*???*/0;
	New -> KSS = 0x10;
	
};
