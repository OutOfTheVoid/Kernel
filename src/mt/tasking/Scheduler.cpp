#include <mt/tasking/Scheduler.h>
#include <mt/tasking/Switcher.h>
#include <mt/tasking/Task.h>
#include <mt/tasking/Idle.h>

#include <hw/cpu/EFlags.h>

#include <mm/KMalloc.h>
#include <mm/PMAlloc.h>

#include <system/func/panic.h>

#include <hw/cpu/Processor.h>

#include <util/string/String.h>

#include <interrupt/InterruptHandlers.h>
#include <interrupt/APIC.h>

MT::Synchronization::Spinlock :: Spinlock_t MT::Tasking::Scheduler :: TIDLock = MT::Synchronization::Spinlock :: Initializer ();
uint64_t MT::Tasking::Scheduler :: MaxID = 0;

MT::Synchronization::Spinlock :: Spinlock_t MT::Tasking::Scheduler :: TTLock = MT::Synchronization::Spinlock :: Initializer ();
MT::Tasking::Task :: Task_t * MT::Tasking::Scheduler :: TaskTable [ 0x20 ];

ASM_LINKAGE void mt_tasking_schedulerInterrupt ( Interrupt::InterruptHandlers :: ISRFrame * Frame );

void MT::Tasking::Scheduler :: Init ()
{
	
	for ( uint32_t I = 0; I < 0x20; I ++ )
		TaskTable [ I ] = NULL;
	
	Interrupt::InterruptHandlers :: SetInterruptHandler ( 0x20, & mt_tasking_schedulerInterrupt );
	
};

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
	
	void * KStack = mm_pmalloc ( 4 );
	
	if ( KStack == NULL )
		KPANIC ( "Failed to allocate KStack for idle task!" );
	
	ThisCPU -> IdleTask -> Flags = MT::Tasking::Task :: kFlag_Kernel;
	ThisCPU -> IdleTask -> State = MT::Tasking::Task :: kState_Runnable;
	ThisCPU -> IdleTask -> WaitAttribute = NULL;
	ThisCPU -> IdleTask -> User = 0;
	ThisCPU -> IdleTask -> Privelege = MT::Tasking::Task :: kPrivelege_Exec | MT::Tasking::Task :: kPrivelege_IO;
	ThisCPU -> IdleTask -> Priority = 0xFFFFFFFF;
	ThisCPU -> IdleTask -> Next = ThisCPU -> IdleTask;
	ThisCPU -> IdleTask -> Previous = ThisCPU -> IdleTask;
	
	ThisCPU -> IdleTask -> KStack = reinterpret_cast <void *> ( reinterpret_cast <uint32_t> ( KStack ) + 0x4000 );
	ThisCPU -> IdleTask -> KSS = 0x10;
	
	ThisCPU -> CurrentTask = ThisCPU -> IdleTask;
	
	Interrupt::APIC :: SetLocalTimerVector ( false, 0x20 );
	Interrupt::APIC :: SetLocalTimerDivide ( Interrupt::APIC :: kTimerDivision_16 );
	
	double SystemClockPeriod = kSchedulingQuantumMS / 1000.0 * Interrupt::APIC :: GetBusFrequencey () / 16.0;
	
	Interrupt::APIC :: StartTimerPeriodic ( static_cast <uint32_t> ( SystemClockPeriod ) );
	
};

void MT::Tasking::Scheduler :: Schedule ()
{
	
	::HW::CPU::Processor :: CPUInfo * ThisCPU = ::HW::CPU::Processor :: GetCurrent ();
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & TTLock );
	
	Task :: Task_t * Next = NULL;
	uint32_t I;
	
	for ( I = 0; I < 0x20; I ++ )
	{
		
		if ( TaskTable [ I ] != NULL )
		{
			
			Next = TaskTable [ I ];
			
			break;
			
		}
		
	}
	
	if ( Next == NULL )
		Next = ThisCPU -> IdleTask;
	
	if ( Next == ThisCPU -> CurrentTask )
	{
		
		MT::Synchronization::Spinlock :: Release ( & TTLock );
		
		return;
		
	}
	
	if ( Next != ThisCPU -> IdleTask )
	{
		
		if ( TaskTable [ I ] -> Next == TaskTable [ I ] )
			TaskTable [ I ] = NULL;
		else
		{
			
			TaskTable [ I ] = Next -> Next;
			TaskTable [ I ] -> Previous = Next -> Previous;
			TaskTable [ I ] -> Previous -> Next = TaskTable [ I ];
			
		}
		
	}
	
	if ( ( ThisCPU -> CurrentTask -> State == Task :: kState_Runnable ) && ThisCPU -> CurrentTask != ThisCPU -> IdleTask )
	{
		
		uint32_t OldPriority = ThisCPU -> CurrentTask -> Priority;
		
		if ( TaskTable [ OldPriority ] != NULL )
		{
			
			TaskTable [ OldPriority ] -> Previous -> Next = ThisCPU -> CurrentTask;
			ThisCPU -> CurrentTask -> Previous = TaskTable [ OldPriority ] -> Previous;
			
			TaskTable [ OldPriority ] -> Previous = ThisCPU -> CurrentTask;
			ThisCPU -> CurrentTask -> Next = TaskTable [ OldPriority ];
			
		}
		else
		{
			
			TaskTable [ OldPriority ] = ThisCPU -> CurrentTask;
			ThisCPU -> CurrentTask -> Next = ThisCPU -> CurrentTask;
			ThisCPU -> CurrentTask -> Previous = ThisCPU -> CurrentTask;
			
		}
		
	}
	
	MT::Synchronization::Spinlock :: Release ( & TTLock );
	
	Switcher :: SwitchTo ( Next, ThisCPU -> CurrentTask );
	
};

uint64_t MT::Tasking::Scheduler :: GetNewTaskID ()
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & TIDLock );
	
	uint64_t Return = MaxID;
	MaxID ++;
	
	MT::Synchronization::Spinlock :: Release ( & TIDLock );
	
	return Return;
	
};

void mt_tasking_schedulerInterrupt ( Interrupt::InterruptHandlers :: ISRFrame * Frame )
{
	
	MT::Tasking::Scheduler :: Schedule ();
	
};
