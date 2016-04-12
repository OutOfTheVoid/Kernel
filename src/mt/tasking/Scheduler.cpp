#include <mt/tasking/Scheduler.h>
#include <mt/tasking/Switcher.h>
#include <mt/tasking/Task.h>
#include <mt/tasking/Idle.h>
#include <mt/tasking/Reaper.h>

#include <hw/cpu/EFlags.h>

#include <mm/KMalloc.h>
#include <mm/PMAlloc.h>

#include <system/func/Panic.h>

#include <hw/cpu/Processor.h>

#include <util/string/String.h>

#include <interrupt/InterruptHandlers.h>
#include <interrupt/APIC.h>
#include <interrupt/IState.h>

#include <cpputil/Linkage.h>
#include <cpputil/Unused.h>

MT::Synchronization::Spinlock :: Spinlock_t MT::Tasking::Scheduler :: TaskIDLock = MT::Synchronization::Spinlock :: Initializer ();
uint64_t MT::Tasking::Scheduler :: MaxID = 0;

MT::Tasking::Task :: Task_t * MT::Tasking::Scheduler :: ReapingListHead = NULL;
MT::Synchronization::Spinlock :: Spinlock_t MT::Tasking::Scheduler :: ReapListLock = MT::Synchronization::Spinlock :: Initializer ();

MT::Synchronization::Spinlock :: Spinlock_t MT::Tasking::Scheduler :: TaskTableLock = MT::Synchronization::Spinlock :: Initializer ();
MT::Tasking::Task :: Task_t * MT::Tasking::Scheduler :: TaskTable [ 0x20 ];

ASM_LINKAGE void mt_tasking_schedulerInterrupt ( Interrupt::InterruptHandlers :: ISRFrame * Frame );

void MT::Tasking::Scheduler :: Init ()
{
	
	for ( uint32_t I = 0; I < 0x20; I ++ )
		TaskTable [ I ] = NULL;
	
	Interrupt::InterruptHandlers :: SetInterruptHandler ( 0x20, & mt_tasking_schedulerInterrupt );
	
	Reaper :: Init ();
	
};

void MT::Tasking::Scheduler :: PInit ( uint32_t Priority )
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
	IDN [ IDNLength ] = 0;
	
	memcpy ( reinterpret_cast <void *> ( CPUInitTask -> Name ), "CPU Init ", 9 );
	memcpy ( reinterpret_cast <void *> ( & CPUInitTask -> Name [ 9 ] ), IDN, IDNLength + 1 );
	
	CPUInitTask -> Flags = MT::Tasking::Task :: kFlag_Kernel | MT::Tasking::Task :: kFlag_Math | MT::Tasking::Task :: kFlag_MathInit | MT::Tasking::Task :: kFlag_CPUInitStack;
	CPUInitTask -> State = MT::Tasking::Task :: kState_Runnable;
	CPUInitTask -> User = 0;
	CPUInitTask -> Privelege = MT::Tasking::Task :: kPrivelege_Exec | MT::Tasking::Task :: kPrivelege_IO;
	CPUInitTask -> Priority = Priority;
	CPUInitTask -> Next = CPUInitTask;
	CPUInitTask -> Previous = CPUInitTask;
	
	CPUInitTask -> KSS = 0x10;
	
	ThisCPU -> CurrentTask = CPUInitTask;
	
	char IdleName [ 20 ];
	
	utoa ( ThisCPU -> Index, IDN, 10 );
	IDNLength = strlen ( IDN );
	IDN [ IDNLength ] = 0;
	
	memcpy ( reinterpret_cast <void *> ( IdleName ), "CPU Idle ", 9 );
	memcpy ( reinterpret_cast <void *> ( & IdleName [ 9 ] ), IDN, IDNLength + 1 );
	
	Task :: Task_t * IdleTask = Task :: CreateKernelTask ( const_cast <const char *> ( IdleName ), reinterpret_cast <void *> ( & mt_tasking_idleEntry ), 0x1000, 0xFFFFFFFF, 0xFFFFFFFF );
	
	ThisCPU -> IdleTask = IdleTask;
	
	Interrupt::APIC :: SetLocalTimerVector ( false, 0x20 );
	Interrupt::APIC :: SetLocalTimerDivide ( Interrupt::APIC :: kTimerDivision_16 );
	
	bool ReInt = Interrupt::IState :: ReadAndSetBlock ();
	
	double SystemClockPeriod = kSchedulingQuantumMS / 1000.0 * Interrupt::APIC :: GetBusFrequencey () / 16.0;
	Interrupt::APIC :: StartTimerPeriodic ( static_cast <uint32_t> ( SystemClockPeriod ) );
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & ThisCPU -> Lock );
	
	ThisCPU -> Flags |= ::HW::CPU::Processor :: kCPUFlag_Scheduling;
	
	MT::Synchronization::Spinlock :: Release ( & ThisCPU -> Lock );
	Interrupt::IState :: WriteBlock ( ReInt );
	
};

void MT::Tasking::Scheduler :: Schedule ()
{
	
	::HW::CPU::Processor :: CPUInfo * ThisCPU = ::HW::CPU::Processor :: GetCurrent ();
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & TaskTableLock );
	
	Task :: Task_t * Last = ThisCPU -> CurrentTask;
	uint32_t Priority = Last -> Priority;
	
	
	
	if ( Last != ThisCPU -> IdleTask )
	{
		
		if ( Last -> State == Task :: kState_Runnable )
		{
			
			if ( Last -> Priority > Task :: kPriority_RealTime_Min )
			{
				
				if ( Last -> Priority < Last -> MinPriority )
					Last -> Priority ++;
				
			}
			
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
		else if ( Last -> State == Task :: kState_RunnableYielded )
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
		else
		{
			
			if ( ( Last -> Priority > Last -> MaxPriority ) && ( Last -> Priority > Task :: kPriority_RealTime_Min ) )
				Last -> Priority --;
			
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
	
	if ( Last -> State == Task :: kState_KernelSelectable )
	{
		
		Last -> Flags |= Task :: kFlag_Suspended;
		
	}
	else if ( Last -> State == Task :: kState_Dead )
	{
		
		Synchronization::Spinlock :: SpinAcquire ( & ReapListLock );
		
		if ( ReapingListHead != NULL )
		{
			
			Last -> Next = ReapingListHead;
			ReapingListHead = Last;
			
		}
		else
		{
			
			Last -> Next = NULL;
			ReapingListHead = Last;
			
		}
		
		Synchronization::Spinlock :: Release ( & ReapListLock );
		
		Reaper :: Signal ();
		
	}
	
	Synchronization::Spinlock :: Release ( & TaskTableLock );
	
	if ( Next != Last )
		Switcher :: SwitchTo ( Next, Last );
	
	if ( ThisCPU -> ReleaseOnSchedule != NULL )
	{
		
		ThisCPU -> ReleaseOnSchedule -> Lock = 0;
		ThisCPU -> ReleaseOnSchedule = NULL;
		
	}
	
};

void MT::Tasking::Scheduler :: AddTask ( Task :: Task_t * ToAdd )
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & TaskTableLock );
	
	AddTaskInternal ( ToAdd );
	
	MT::Synchronization::Spinlock :: Release ( & TaskTableLock );
	
};

void MT::Tasking::Scheduler :: AddTaskInternal ( Task :: Task_t * ToAdd )
{
	
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
	
};

void MT::Tasking::Scheduler :: KillTask ( Task :: Task_t * ToKill )
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & TaskTableLock );
	
	ToKill -> State = Task :: kState_Dead;
	
	MT::Synchronization::Spinlock :: Release ( & TaskTableLock );
	
};

void MT::Tasking::Scheduler :: SuspendTask ( Task :: Task_t * ToSuspend )
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & TaskTableLock );
	
	ToSuspend -> State = Task :: kState_KernelSelectable;
	
	MT::Synchronization::Spinlock :: Release ( & TaskTableLock );
	
};

void MT::Tasking::Scheduler :: KillCurrentTask ()
{
	
	bool ReInt = Interrupt::IState :: ReadAndSetBlock ();
	
	::HW::CPU::Processor :: CPUInfo * ThisCPU = ::HW::CPU::Processor :: GetCurrent ();
	Task :: Task_t * ThisTask = ThisCPU -> CurrentTask;
	
	ThisTask -> State = Task :: kState_Dead;
	
	Interrupt::IState :: WriteBlock ( ReInt );
	
	Preemt ();
	
};

void MT::Tasking::Scheduler :: YieldCurrentTask ()
{
	
	bool ReInt = Interrupt::IState :: ReadAndSetBlock ();
	
	::HW::CPU::Processor :: CPUInfo * ThisCPU = ::HW::CPU::Processor :: GetCurrent ();
	Task :: Task_t * ThisTask = ThisCPU -> CurrentTask;
	
	ThisTask -> State = Task :: kState_RunnableYielded;
	
	Interrupt::IState :: WriteBlock ( ReInt );
	
	Preemt ();
	
};

void MT::Tasking::Scheduler :: SuspendCurrentTask ()
{
	
	bool ReInt = Interrupt::IState :: ReadAndSetBlock ();
	
	::HW::CPU::Processor :: CPUInfo * ThisCPU = ::HW::CPU::Processor :: GetCurrent ();
	Task :: Task_t * ThisTask = ThisCPU -> CurrentTask;
	
	ThisTask -> State = Task :: kState_KernelSelectable;
	
	Interrupt::IState :: WriteBlock ( ReInt );
	
	Preemt ();
	
};

MT::Tasking::Task :: Task_t * MT::Tasking::Scheduler :: GetNextDeadTask ()
{
	
	Task :: Task_t * DeadTask = NULL;
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & ReapListLock );
	
	if ( ReapingListHead != NULL )
	{
		
		DeadTask = ReapingListHead;
		ReapingListHead = DeadTask -> Next;
		
	}
	
	MT::Synchronization::Spinlock :: Release ( & ReapListLock );
	
	return DeadTask;
	
};

uint64_t MT::Tasking::Scheduler :: GetNewTaskID ()
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & TaskIDLock );
	
	uint64_t Return = MaxID;
	MaxID ++;
	
	MT::Synchronization::Spinlock :: Release ( & TaskIDLock );
	
	return Return;
	
};

void mt_tasking_schedulerInterrupt ( Interrupt::InterruptHandlers :: ISRFrame * Frame )
{
	
	UNUSED ( Frame );
	
	Interrupt::APIC :: EndOfInterrupt ();
	
	MT::Tasking::Scheduler :: Schedule ();
	
};
