#include <mt/tasking/Scheduler.h>
#include <mt/tasking/Switcher.h>
#include <mt/tasking/Task.h>
#include <mt/tasking/Idle.h>
#include <mt/tasking/Reaper.h>

#include <hw/cpu/EFlags.h>

#include <mm/KMalloc.h>
#include <mm/PMAlloc.h>

#include <system/func/Panic.h>
#include <system/func/KPrintF.h>

#include <hw/cpu/Processor.h>

#include <util/string/String.h>

#include <interrupt/InterruptHandlers.h>
#include <interrupt/APIC.h>
#include <interrupt/IState.h>

#include <cpputil/Linkage.h>
#include <cpputil/Unused.h>

MT::Synchronization::Spinlock :: Spinlock_t MT::Tasking::Scheduler :: TaskIDLock = MT::Synchronization::Spinlock :: Initializer ();
uint64_t MT::Tasking::Scheduler :: MaxID = 0;

volatile MT::Tasking::Task * MT::Tasking::Scheduler :: ReapingListHead = NULL;
MT::Synchronization::Spinlock :: Spinlock_t MT::Tasking::Scheduler :: ReapListLock = MT::Synchronization::Spinlock :: Initializer ();

MT::Synchronization::Spinlock :: Spinlock_t MT::Tasking::Scheduler :: TaskTableLock = MT::Synchronization::Spinlock :: Initializer ();
volatile MT::Tasking::Task * MT::Tasking::Scheduler :: TaskTable [ 0x20 ];

ASM_LINKAGE void mt_tasking_schedulerInterrupt ( Interrupt::InterruptHandlers :: ISRFrame * Frame );

void MT::Tasking::Scheduler :: Init ()
{
	
	for ( uint32_t I = 0; I < 0x20; I ++ )
		TaskTable [ I ] = NULL;
	
	Interrupt::InterruptHandlers :: SetInterruptHandler ( 0x20, & mt_tasking_schedulerInterrupt );
	
	Switcher :: Init ( & TaskTableLock );
	
	Reaper :: Init ();
	
};

void MT::Tasking::Scheduler :: PInit ( uint32_t Priority )
{
	
	::HW::CPU::Processor :: CPUInfo * ThisCPU = ::HW::CPU::Processor :: GetCurrent ();
	
	Task * CPUInitTask = reinterpret_cast <MT::Tasking::Task *> ( mm_kmalloc ( sizeof ( Task ) ) );
	
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
	
	volatile Task * IdleTask = Task :: CreateKernelTask ( const_cast <const char *> ( IdleName ), reinterpret_cast <void *> ( & mt_tasking_idleEntry ), 0x1000, 0xFFFFFFFF, 0xFFFFFFFF );
	
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

void MT::Tasking::Scheduler :: Schedule ( Interrupt::InterruptHandlers :: ISRFrame * Frame )
{
	
	::HW::CPU::Processor :: CPUInfo * ThisCPU = ::HW::CPU::Processor :: GetCurrent ();
	
	volatile Task * Last = ThisCPU -> CurrentTask;
	volatile Task * Next = NULL;
	
	if ( Last != ThisCPU -> IdleTask )
	{
		
		if ( ( Last -> State == Task :: kState_Runnable ) && ( Last -> Priority > Task :: kPriority_RealTime_Min ) )
		{
			
			if ( Last -> Priority < Last -> MinPriority )
				Last -> Priority ++;
			
		}
		else if ( Last -> State == Task :: kState_RunnableYielded )
		{
			
			Last -> State = Task :: kState_Runnable;
			
		}
		else if ( ( Last -> Priority > Last -> MaxPriority ) && ( Last -> Priority > Task :: kPriority_RealTime_Min ) )
			Last -> Priority --;
		
	}
	
	if ( ThisCPU -> ReleaseOnSchedule != NULL )
	{
		
		ThisCPU -> ReleaseOnSchedule -> Lock = 0;
		ThisCPU -> ReleaseOnSchedule = NULL;
		
	}
	
	if ( ( Last -> Flags & Task :: kFlag_Kernel ) == 0 )
		Task :: SyncUserStack ( Last, Frame );
	
	Synchronization::Spinlock :: SpinAcquire ( & TaskTableLock );
	
	uint32_t I = 0;
	
	while ( I < 20 )
	{
		
		if ( ( Last -> Priority < I ) && ( Last -> State == Task :: kState_Runnable ) && ( Last != ThisCPU -> IdleTask ) )
		{
			
			Next = Last;
			
			break;
			
		}
		
		if ( TaskTable [ I ] != NULL )
		{
			
			Next = TaskTable [ I ];
			
			break;
			
		}
		
		I ++;
		
	}
	
	if ( Next == NULL )
		Next = ThisCPU -> IdleTask;
	else if ( Next != Last )
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
	
	if ( ( Last != Next ) && ( Last != ThisCPU -> IdleTask ) && ( Last -> State == Task :: kState_Runnable ) )
		AddTaskInternal ( Last );
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
	
	ThisCPU -> CurrentTask = Next;
	
	if ( ( Next -> Flags & Task :: kFlag_Kernel ) == 0 )
	{
		
		ThisCPU -> CrossPrivelegeInterruptTSS.ESP0 = reinterpret_cast <uint32_t> ( Next -> KStack );
		
		ThisCPU -> CrossPrivelegeInterruptTSS.SS0 = Next -> KSS;
		ThisCPU -> CrossPrivelegeInterruptTSS.SS0_High = 0;
		
	}
	
	if ( Next -> MemoryMapping != Last -> MemoryMapping )
	{
		
		if ( Next -> MemoryMapping != NULL )
			Next -> MemoryMapping -> Load ();
		else
			MM::Paging::PageTable :: KLoad ();
		
	}
	
	if ( Next != Last )
		Switcher :: SwitchTo ( const_cast <Task *> ( Next ), Last );
	else
		Synchronization::Spinlock :: Release ( & TaskTableLock );
	
};

void MT::Tasking::Scheduler :: AddTask ( Task * ToAdd )
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & TaskTableLock );
	
	AddTaskInternal ( ToAdd );
	
	MT::Synchronization::Spinlock :: Release ( & TaskTableLock );
	
};

void MT::Tasking::Scheduler :: AddTaskInternal ( volatile Task * ToAdd )
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

void MT::Tasking::Scheduler :: KillTask ( Task * ToKill )
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & TaskTableLock );
	
	ToKill -> State = Task :: kState_Dead;
	
	MT::Synchronization::Spinlock :: Release ( & TaskTableLock );
	
};

void MT::Tasking::Scheduler :: SuspendTask ( Task * ToSuspend )
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & TaskTableLock );
	
	ToSuspend -> State = Task :: kState_KernelSelectable;
	
	MT::Synchronization::Spinlock :: Release ( & TaskTableLock );
	
};

void MT::Tasking::Scheduler :: KillCurrentTask ()
{
	
	bool ReInt = Interrupt::IState :: ReadAndSetBlock ();
	
	::HW::CPU::Processor :: CPUInfo * ThisCPU = ::HW::CPU::Processor :: GetCurrent ();
	volatile Task * ThisTask = ThisCPU -> CurrentTask;
	
	ThisTask -> State = Task :: kState_Dead;
	
	Interrupt::IState :: WriteBlock ( ReInt );
	
	Preemt ();
	
};

void MT::Tasking::Scheduler :: YieldCurrentTask ()
{
	
	bool ReInt = Interrupt::IState :: ReadAndSetBlock ();
	
	::HW::CPU::Processor :: CPUInfo * ThisCPU = ::HW::CPU::Processor :: GetCurrent ();
	volatile Task * ThisTask = ThisCPU -> CurrentTask;
	
	ThisTask -> State = Task :: kState_RunnableYielded;
	
	Interrupt::IState :: WriteBlock ( ReInt );
	
	Preemt ();
	
};

void MT::Tasking::Scheduler :: SuspendCurrentTask ()
{
	
	bool ReInt = Interrupt::IState :: ReadAndSetBlock ();
	
	::HW::CPU::Processor :: CPUInfo * ThisCPU = ::HW::CPU::Processor :: GetCurrent ();
	volatile Task * ThisTask = ThisCPU -> CurrentTask;
	
	ThisTask -> State = Task :: kState_KernelSelectable;
	
	Preemt ();
	
	Interrupt::IState :: WriteBlock ( ReInt );
	
};

MT::Tasking::Task * MT::Tasking::Scheduler :: GetNextDeadTask ()
{
	
	volatile Task * DeadTask = NULL;
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & ReapListLock );
	
	if ( ReapingListHead != NULL )
	{
		
		DeadTask = ReapingListHead;
		ReapingListHead = DeadTask -> Next;
		
	}
	
	MT::Synchronization::Spinlock :: Release ( & ReapListLock );
	
	return const_cast <Task *> ( DeadTask );
	
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
	
	Interrupt::APIC :: EndOfInterrupt ();
	
	MT::Tasking::Scheduler :: Schedule ( Frame );
	
};
