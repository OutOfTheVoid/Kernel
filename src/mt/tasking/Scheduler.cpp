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

#include <cpputil/Linkage.h>
#include <cpputil/Unused.h>

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
	
	Task :: Task_t * CPUInitTask = reinterpret_cast <MT::Tasking::Task :: Task_t *> ( mm_kmalloc ( sizeof ( Task :: Task_t ) ) );
	
	if ( CPUInitTask == NULL )
		KPANIC ( "Failed to allocate idle task for processor!" );
	
	CPUInitTask -> ID = GetNewTaskID ();
	
	char IDN [ 6 ];
	uint32_t IDNLength;
	
	utoa ( ThisCPU -> Index, IDN, 10 );
	IDNLength = strlen ( IDN );
	
	memcpy ( reinterpret_cast <void *> ( CPUInitTask -> Name ), "CPU Init ", 9 );
	memcpy ( reinterpret_cast <void *> ( & CPUInitTask -> Name [ 9 ] ), IDN, IDNLength );
	
	void * KStack = mm_pmalloc ( 4 );
	
	if ( KStack == NULL )
		KPANIC ( "Failed to allocate KStack for idle task!" );
	
	CPUInitTask -> Flags = MT::Tasking::Task :: kFlag_Kernel;
	CPUInitTask -> State = MT::Tasking::Task :: kState_Runnable;
	CPUInitTask -> WaitAttribute = NULL;
	CPUInitTask -> User = 0;
	CPUInitTask -> Privelege = MT::Tasking::Task :: kPrivelege_Exec | MT::Tasking::Task :: kPrivelege_IO;
	CPUInitTask -> Priority = 0;
	CPUInitTask -> Next = CPUInitTask;
	CPUInitTask -> Previous = CPUInitTask;
	
	CPUInitTask -> KStack = reinterpret_cast <void *> ( reinterpret_cast <uint32_t> ( KStack ) + 0x4000 );
	CPUInitTask -> KSS = 0x10;
	
	ThisCPU -> CurrentTask = CPUInitTask;
	
	char Name [ 20 ];
	uint32_t NameLength;
	
	memcpy ( reinterpret_cast <void *> ( Name ), reinterpret_cast <const void *> ( "CPU Idle ( " ), 11 );
	utoa ( ThisCPU -> Index, & Name [ 11 ], 10 );
	NameLength = strlen ( Name );
	memcpy ( reinterpret_cast <void *> ( & Name [ NameLength ] ), reinterpret_cast <const void *> ( ")" ), 2 );
	
	Task :: Task_t * IdleTask = Task :: CreateKernelTask ( const_cast <const char *> ( Name ), reinterpret_cast <void *> ( & mt_tasking_idleEntry ), 0x1000, 0xFFFFFFFF );
	
	ThisCPU -> IdleTask = IdleTask;
	
	Interrupt::APIC :: SetLocalTimerVector ( false, 0x20 );
	Interrupt::APIC :: SetLocalTimerDivide ( Interrupt::APIC :: kTimerDivision_16 );
	
	//double SystemClockPeriod = kSchedulingQuantumMS / 1000.0 * Interrupt::APIC :: GetBusFrequencey () / 16.0;
	double SystemClockPeriod = 0.5 * Interrupt::APIC :: GetBusFrequencey () / 16.0;
	
	Interrupt::APIC :: StartTimerPeriodic ( static_cast <uint32_t> ( SystemClockPeriod ) );
	
};

void MT::Tasking::Scheduler :: Schedule ()
{
	
	::HW::CPU::Processor :: CPUInfo * ThisCPU = ::HW::CPU::Processor :: GetCurrent ();
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & TTLock );
	
	Task :: Task_t * Last = ThisCPU -> CurrentTask;
	uint32_t Priority = Last -> Priority;
	
	if ( Last != ThisCPU -> IdleTask )
	{
		
		if ( TaskTable [ Priority ] != NULL )
		{
			
			TaskTable [ Priority ] -> Previous -> Next = Last;
			Last -> Previous = TaskTable [ Priority ] -> Previous;
			TaskTable [ Priority ] -> Previous = Last;
			Last -> Next = TaskTable [ Priority ];
			
		}
		else
		{
			
			TaskTable [ Priority ] = Last;
			Last -> Next = Last;
			Last -> Previous = Last;
			
		}
		
	}
	
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
	else
	{
		
		if ( TaskTable [ I ] -> Next == TaskTable [ I ] )
			TaskTable [ I ] = NULL;
		else
		{
			
			Next -> Previous -> Next = Next -> Next;
			Next -> Next -> Previous = Next -> Previous;
			TaskTable [ I ] = Next -> Next;
			
		}
		
	}
	
	ThisCPU -> CurrentTask = Next;
	
	MT::Synchronization::Spinlock :: Release ( & TTLock );
	
	Switcher :: SwitchTo ( Next, Last );
	
};

void MT::Tasking::Scheduler :: AddTask ( Task :: Task_t * ToAdd )
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & TTLock );
	
	uint32_t Priority = ToAdd -> Priority;
	
	if ( TaskTable [ ToAdd -> Priority ] != NULL )
	{
		
		TaskTable [ Priority ] -> Previous -> Next = ToAdd;
		ToAdd -> Previous = TaskTable [ Priority ] -> Previous;
		TaskTable [ Priority ] -> Previous = ToAdd;
		ToAdd -> Next = TaskTable [ Priority ];
		
	}
	else
	{
		
		TaskTable [ Priority ] = ToAdd;
		ToAdd -> Next = ToAdd;
		ToAdd -> Previous = ToAdd;
		
	}
	
	MT::Synchronization::Spinlock :: Release ( & TTLock );
	
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
	
	UNUSED ( Frame );
	
	Interrupt::APIC :: EndOfInterrupt ();
	
	MT::Tasking::Scheduler :: Schedule ();
	
};
