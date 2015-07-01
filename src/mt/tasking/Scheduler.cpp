#include <mt/tasking/Scheduler.h>
#include <mt/tasking/Task.h>

#include <mm/KMalloc.h>
#include <mm/PMAlloc.h>

#include <system/func/panic.h>

#include <hw/cpu/Processor.h>

#include <util/string/String.h>

MT::Synchronization::Spinlock :: Spinlock_t MT::Tasking::Scheduler :: TIDLock = MT::Synchronization::Spinlock :: Initializer ();
uint64_t MT::Tasking::Scheduler :: MaxID = 0;

MT::Tasking::Task :: Task_t * MT::Tasking::Scheduler :: ProccessTable [ 0x20 ];

void MT::Tasking::Scheduler :: PInit ()
{
	
	::HW::CPU::Processor :: CPUInfo * ThisCPU = ::HW::CPU::Processor :: GetCurrent ();
	
	ThisCPU -> IdleTask = reinterpret_cast <MT::Tasking::Task :: Task_t *> ( mm_kmalloc ( sizeof ( Task :: Task_t ) ) );
	
	if ( ThisCPU -> IdleTask == NULL )
		KPANIC ( "Failed to allocate idle task for processor!" );
	
	ThisCPU -> IdleTask -> ID = GetNewTaskID ();
	
	char IDN [ 6 ];
	uint32_t IDNLength;
	
	utoa ( ThisCPU -> Index, IDN, 10 );
	IDNLength = strlen ( IDN );
	
	memcpy ( reinterpret_cast <void *> ( ThisCPU -> IdleTask -> Name ), "cpu idle ( ", 11 );
	memcpy ( reinterpret_cast <void *> ( & ThisCPU -> IdleTask -> Name [ 11 ] ), IDN, IDNLength );
	memcpy ( reinterpret_cast <void *> ( & ThisCPU -> IdleTask -> Name [ 11 + IDNLength ] ), " )\0", 3 );
	
	ThisCPU -> IdleTask -> Flags = MT::Tasking::Task :: kFlag_Kernel;
	ThisCPU -> IdleTask -> State = MT::Tasking::Task :: kState_Runnable;
	ThisCPU -> IdleTask -> WaitAttribute = NULL;
	ThisCPU -> IdleTask -> User = 0;
	ThisCPU -> IdleTask -> Privelege = MT::Tasking::Task :: kPrivelege_Exec | MT::Tasking::Task :: kPrivelege_IO;
	ThisCPU -> IdleTask -> Priority = 0xFFFFFFFF;
	ThisCPU -> IdleTask -> Next = ThisCPU -> IdleTask;
	
	
	
};

void MT::Tasking::Scheduler :: Schedule ()
{
	
	
	
};

uint64_t MT::Tasking::Scheduler :: GetNewTaskID ()
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & TIDLock );
	
	uint64_t Return = MaxID;
	MaxID ++;
	
	MT::Synchronization::Spinlock :: Release ( & TIDLock );
	
	return Return;
	
};