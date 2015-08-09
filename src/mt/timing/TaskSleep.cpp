#include <mt/timing/TaskSleep.h>
#include <mt/timing/PIT.h>

#include <mt/tasking/Scheduler.h>

#include <mm/KMalloc.h>

#include <hw/cpu/Processor.h>
#include <hw/cpu/TSC.h>
#include <hw/acpi/PMTimer.h>

#include <interrupt/IState.h>
#include <interrupt/APIC.h>
#include <interrupt/InterruptHandlers.h>

#include <cpputil/Linkage.h>

#include <system/func/Panic.h>

MT::Timing::TaskSleep :: SleepInfo * MT::Timing::TaskSleep :: TreeRoot = NULL;
MT::Synchronization::Spinlock :: Spinlock_t MT::Timing::TaskSleep :: TreeLock = MT::Synchronization::Spinlock :: Initializer ();
MT::Synchronization::Spinlock :: Spinlock_t MT::Timing::TaskSleep :: TimerLock = MT::Synchronization::Spinlock :: Initializer ();

MT::Synchronization::Spinlock :: Spinlock_t MT::Timing::TaskSleep :: FreeListLock = MT::Synchronization::Spinlock :: Initializer ();
MT::Timing::TaskSleep :: SleepInfo * MT::Timing::TaskSleep :: FreeHead = NULL;
uint32_t MT::Timing::TaskSleep :: FreeCount = 0;

uint64_t MT::Timing::TaskSleep :: NextInterrupt = 0;

MT::Timing::TaskSleep :: TimingSource MT::Timing::TaskSleep :: TimeSource;
MT::Timing::TaskSleep :: InterruptSource MT::Timing::TaskSleep :: IntSource;

ASM_LINKAGE void mt_timing_tasksleep_pitirqhandler ( Interrupt::InterruptHandlers :: ISRFrame * Frame );

void MT::Timing::TaskSleep :: Init ()
{
	
	if ( ::HW::ACPI::PMTimer :: Exists () )
		TimeSource = kTimingSource_PMTimer;
	else
		TimeSource = kTimingSource_TSC;
	
	IntSource = kInterruptSource_PIT;
	
	PIT :: InitTimedIRQ ( & mt_timing_tasksleep_pitirqhandler );
	PIT :: SetIRQEnabled ( true );
	
	NextInterrupt = 0xFFFFFFFFFFFFFFFF;
	
};

uint64_t MT::Timing::TaskSleep ::  GetCurrentTimeNS ()
{
	
	switch ( TimeSource )
	{
	
	case kTimingSource_TSC:
		return static_cast <uint64_t> ( ::HW::CPU::TSC :: Read () / ( Interrupt::APIC :: GetBusFrequencey () / 1000000000.0 ) );
		
	case kTimingSource_PMTimer:
		return ::HW::ACPI::PMTimer :: GetTimeNS ();
		
	default:
		return 0;
		
	}
	
};

inline uint64_t MT::Timing::TaskSleep :: DelayMSToNS ( double MS )
{
	
	return static_cast <uint64_t> ( MS * 1000000.0 );
	
};

void MT::Timing::TaskSleep :: SetNextInterrupt ( uint64_t TimeoutNS )
{
	
	switch ( IntSource )
	{
	
	case kInterruptSource_PIT:
		{
			
			if ( TimeoutNS > 50000000 )
				TimeoutNS = 50000000;
			
			PIT :: SetTimeoutMS ( static_cast <double> ( TimeoutNS ) / 1000000.0 );
			
		}
		
		break;
		
	default:
	
		break;
	
	}
	
};

void MT::Timing::TaskSleep :: SleepCurrent ( double Milliseconds )
{
	
	if ( Milliseconds <= 0.0 )
		return;
	
	SleepInfo * NewNode = CreateNode ();
	
	bool ReInt = Interrupt::IState :: ReadAndSetBlock ();
	
	if ( NewNode == NULL )
		KPANIC ( "Failed to create new wait node!" );
	
	::HW::CPU::Processor :: CPUInfo * ThisCPU = ::HW::CPU::Processor :: GetCurrent ();
	
	Tasking::Task :: Task_t * PendingTask = ThisCPU -> CurrentTask;
	PendingTask -> State = Tasking::Task :: kState_Blocked;
	NewNode -> PendingTask = PendingTask;
	
	uint64_t Current = GetCurrentTimeNS ();
	uint64_t Delay = DelayMSToNS ( Milliseconds );
	uint64_t ThresholdNS = Current + Delay;
	NewNode -> ThresholdNS = ThresholdNS;
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & TreeLock );
	InsertNode ( NewNode );
	MT::Synchronization::Spinlock :: Release ( & TreeLock );
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & TimerLock );
	
	if ( NextInterrupt > ThresholdNS )
	{
		
		NextInterrupt = ThresholdNS;
		
		SetNextInterrupt ( Delay );
			
	}
	
	MT::Synchronization::Spinlock :: Release ( & TimerLock );
	
	Interrupt::IState :: WriteBlock ( ReInt );
	
	__asm__ volatile ( "int 0x20" );
	
};

void MT::Timing::TaskSleep :: OnInterrupt ()
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & TreeLock );
	
	uint64_t Time = GetCurrentTimeNS ();
	uint64_t NextInterruptUpdated = 0xFFFFFFFFFFFFFFFF;
	
	SleepInfo * WakeHead = NULL;
	SleepInfo * LeftMost = ( TreeRoot != NULL ? __LeftMostNode ( TreeRoot ) : NULL );
	
	while ( LeftMost != NULL )
	{
		
		if ( Time >= LeftMost -> ThresholdNS )
		{
			
			DeleteNode ( LeftMost );
			
			if ( WakeHead == NULL )
			{
				
				WakeHead = LeftMost;
				WakeHead -> Right = NULL;
				
			}
			else
			{
				
				LeftMost -> Right = WakeHead;
				WakeHead = LeftMost;
				
			}
			
			LeftMost = ( TreeRoot != NULL ? __LeftMostNode ( TreeRoot ) : NULL );
			
		}
		else
		{
			
			NextInterruptUpdated = LeftMost -> ThresholdNS - Time;
			
			break;
			
		}
		
	}
	
	MT::Synchronization::Spinlock :: Release ( & TreeLock );
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & TimerLock );
	SetNextInterrupt ( NextInterruptUpdated );
	NextInterrupt = NextInterruptUpdated;
	MT::Synchronization::Spinlock :: Release ( & TimerLock );
	
	SleepInfo * Current = NULL;
	
	while ( WakeHead != NULL )
	{
		
		Current = WakeHead;
		
		Tasking::Task :: Task_t * WakeTask = Current -> PendingTask;
		WakeHead = Current -> Right;
		DestroyNode ( Current );
		
		WakeTask -> State = Tasking::Task :: kState_Runnable;
		MT::Tasking::Scheduler :: AddTask ( WakeTask );
		
	}
	
};

void mt_timing_tasksleep_pitirqhandler ( Interrupt::InterruptHandlers :: ISRFrame * Frame )
{
	
	(void) Frame;
	
	MT::Timing::TaskSleep :: OnInterrupt ();
	
	Interrupt::APIC :: EndOfInterrupt ();
	
};

MT::Timing::TaskSleep :: SleepInfo * MT::Timing::TaskSleep :: CreateNode ()
{
	
	SleepInfo * New = NULL;
	
	bool ReInt = Interrupt::IState :: ReadAndSetBlock ();
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & FreeListLock );
	
	if ( FreeCount != 0 )
	{
		
		New = FreeHead;
		FreeHead = New -> Right;
		
		FreeCount --;
		
	}
	
	MT::Synchronization::Spinlock :: Release ( & FreeListLock );
	
	Interrupt::IState :: WriteBlock ( ReInt );
	
	if ( New == NULL )
		New = reinterpret_cast <SleepInfo *> ( mm_kmalloc ( sizeof ( SleepInfo ) ) );
	
	if ( New == NULL )
		return NULL;
	
	New -> Parent = NULL;
	New -> Left = NULL;
	New -> Right = NULL;
	
	New -> Height = 0;
	
	return New;
	
};

void MT::Timing::TaskSleep :: DestroyNode ( SleepInfo * Node )
{
	
	bool ReInt = Interrupt::IState :: ReadAndSetBlock ();
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & FreeListLock );
	
	Node -> Right = FreeHead;
	FreeHead = Node;
	
	FreeCount ++;
	
	MT::Synchronization::Spinlock :: Release ( & FreeListLock );
	
	Interrupt::IState :: WriteBlock ( ReInt );
	
};

void MT::Timing::TaskSleep :: InsertNode ( SleepInfo * Node )
{
	
	Node -> Height = 0;
	
	if ( TreeRoot == NULL )
	{
		
		TreeRoot = Node;
		
		return;
		
	}
	
	SleepInfo * Current = TreeRoot;
	
	while ( true )
	{
		
		if ( Node -> ThresholdNS >= Current -> ThresholdNS )
		{
			
			if ( Current -> Right == NULL )
			{
				
				Node -> Parent = Current;
				Current -> Right = Node;
				
				break;
				
			}
			
			if ( Current == Current -> Right )
			{
				
				while ( true );
				
			}
			
			Current = Current -> Right;
			
		}
		else
		{
			
			if ( Current -> Left == NULL )
			{
				
				Node -> Parent = Current;
				Current -> Left = Node;
				
				break;
				
			}
			
			if ( Current == Current -> Left )
			{
				
				while ( true );
				
			}
			
			Current = Current -> Left;
			
			
		}
		
	}
	
	Current = Node;
	
	do
	{
		
		Current = Current -> Parent;
		__AdjustHeight ( Current );
		
		if ( Current == TreeRoot )
		{
			
			TreeRoot = __Balance ( Current );
			
			return;
			
		}
		
		Current = __Balance ( Current );
		
	} 
	while ( Current -> Parent != NULL );
	
};

void MT::Timing::TaskSleep :: DeleteNode ( SleepInfo * Node )
{
	
	SleepInfo * UnbalancedNode;
	SleepInfo * SuccessorNode;
	
	if ( Node == TreeRoot )
	{
		
		if ( Node -> Left != NULL )
		{
			
			if ( Node -> Right != NULL )
			{
				
				SuccessorNode = __LeftMostNode ( Node -> Right );
				UnbalancedNode = SuccessorNode -> Parent;
				
				if ( SuccessorNode -> Right != NULL )
				{
					
					if ( UnbalancedNode -> Right == SuccessorNode )
						UnbalancedNode -> Right = SuccessorNode -> Right;
					else
						UnbalancedNode -> Left = SuccessorNode -> Right;
						
					SuccessorNode -> Right -> Parent = UnbalancedNode;
					
				}
				else
				{
					
					if ( UnbalancedNode -> Right == SuccessorNode )
						UnbalancedNode -> Right = NULL;
					else
						UnbalancedNode -> Left = NULL;
					
				}
				
				if ( UnbalancedNode == TreeRoot )
				{
					
					UnbalancedNode = SuccessorNode;
					TreeRoot -> Right = NULL;
					
				}
				
				TreeRoot = SuccessorNode;
				SuccessorNode -> Parent = NULL;
				
				SuccessorNode -> Left = Node -> Left;
				Node -> Left -> Parent = SuccessorNode;
				
				SuccessorNode -> Right = Node -> Right;
				if ( Node -> Right != NULL )
					Node -> Right -> Parent = SuccessorNode;
				
			}
			else
			{
				
				TreeRoot = Node -> Left;
				TreeRoot -> Parent = NULL;
				Node -> Left = NULL;
				
				return;
				
			}
			
		}
		else
		{
			
			if ( Node -> Right != NULL )
			{
				
				TreeRoot = Node -> Right;
				TreeRoot -> Parent = NULL;
				Node -> Right = NULL;
				
				return;
				
			}
			else
			{
				
				TreeRoot = NULL;
				
				return;
				
			}
			
		}
		
	}
	else
	{
		
		if ( Node -> Left != NULL )
		{
			
			if ( Node -> Right != NULL )
			{
				
				SuccessorNode = __LeftMostNode ( Node -> Right );
				UnbalancedNode = SuccessorNode -> Parent;
				
				if ( SuccessorNode -> Right != NULL )
				{
					
					if ( UnbalancedNode -> Left == SuccessorNode )
						UnbalancedNode -> Left = SuccessorNode -> Right;
					else
						UnbalancedNode -> Right = SuccessorNode -> Right;
					
					SuccessorNode -> Right -> Parent = SuccessorNode -> Parent;
					
				}
				else
				{
					
					if ( UnbalancedNode -> Right == SuccessorNode )
						UnbalancedNode -> Right = NULL;
					else
						UnbalancedNode -> Left = NULL;
					
				}
				
				if ( UnbalancedNode == Node )
				{
					
					UnbalancedNode = SuccessorNode;
					Node -> Right = NULL;
					
				}
				
				if ( Node -> Parent -> Left == Node )
					Node -> Parent -> Left = SuccessorNode;
				else
					Node -> Parent -> Right = SuccessorNode;
				
				SuccessorNode -> Parent = Node -> Parent;
				
				SuccessorNode -> Right = Node -> Right;
				if ( SuccessorNode -> Right != NULL )
					SuccessorNode -> Right -> Parent = SuccessorNode;
				
				SuccessorNode -> Left = Node -> Left;
				SuccessorNode -> Left -> Parent = SuccessorNode;
				
				Node -> Parent = NULL;
				Node -> Left = NULL;
				Node -> Right = NULL;
				
			}
			else
			{
				
				if ( Node -> Parent -> Right == Node )
					Node -> Parent -> Right = Node -> Left;
				else
					Node -> Parent -> Left = Node -> Left;
				
				UnbalancedNode = Node -> Left -> Parent = Node -> Parent;
				Node -> Parent = NULL;
				Node -> Left = NULL;
			}
			
		}
		else
		{
			
			if ( Node -> Right != NULL )
			{
				
				if ( Node -> Parent -> Right == Node )
					Node -> Parent -> Right = Node -> Right;
				else
					Node -> Parent -> Left = Node -> Right;
				
				UnbalancedNode = Node -> Right -> Parent = Node -> Parent;
				Node -> Parent = NULL;
				Node -> Right = NULL;
				
			}
			else
			{
				
				if ( Node -> Parent -> Right == Node )
					Node -> Parent -> Right = NULL;
				else
					Node -> Parent -> Left = NULL;
				
				UnbalancedNode = Node -> Parent;
				Node -> Parent = NULL;
				
			}
			
		}
		
	}
	
	while ( true )
	{
		
		__AdjustHeight ( UnbalancedNode );
		
		if ( UnbalancedNode == TreeRoot )
		{
			
			TreeRoot = __Balance ( UnbalancedNode );
			
			return;
			
		}
		
		UnbalancedNode = __Balance ( UnbalancedNode );
		UnbalancedNode = UnbalancedNode -> Parent;
		
	}
	
};

inline int32_t Max ( int32_t A, int32_t B )
{
	
	return ( A > B ) ? A : B;
	
};

inline int32_t MT::Timing::TaskSleep :: __AdjustHeight ( SleepInfo * Node )
{
	
	Node -> Height = 1 + Max ( ( Node -> Right != NULL ) ? Node -> Right -> Height : - 1, ( Node -> Left != NULL ) ? Node -> Left -> Height : - 1 );
	return Node -> Height;
	
};

inline MT::Timing::TaskSleep :: SleepInfo * MT::Timing::TaskSleep :: __RotateRight ( SleepInfo * Root )
{
	
	SleepInfo * NewRoot = Root -> Left;
	
	if ( Root -> Parent != NULL )
	{
		
		if ( Root -> Parent -> Left == Root ) 
			Root -> Parent -> Left = NewRoot; 
		else
			Root -> Parent -> Right = NewRoot;
		
	}
	
	NewRoot -> Parent = Root -> Parent;
	Root -> Parent = NewRoot;
	Root -> Left = NewRoot -> Right;
	
	if ( Root -> Left != NULL )
		Root -> Left -> Parent = Root;
	
	NewRoot -> Right = Root;
	
	__AdjustHeight ( Root );
	__AdjustHeight ( NewRoot );
	
	return NewRoot;
	
};

inline MT::Timing::TaskSleep :: SleepInfo * MT::Timing::TaskSleep :: __LeftMostNode ( SleepInfo * Node )
{
	
	while ( Node -> Left != NULL )
		Node = Node -> Left;
	
	return Node;
	
};

inline MT::Timing::TaskSleep :: SleepInfo * MT::Timing::TaskSleep :: __RotateLeft ( SleepInfo * Root )
{
	
	SleepInfo * NewRoot = Root -> Right;
	
	if ( Root -> Parent != NULL )
	{
		
		if ( Root -> Parent -> Right == Root ) 
			Root -> Parent -> Right = NewRoot; 
		else
			Root -> Parent -> Left = NewRoot;
		
	}
	
	NewRoot -> Parent = Root -> Parent;
	Root -> Parent = NewRoot;
	Root -> Right = NewRoot -> Left;
	
	if ( Root -> Right != NULL )
		Root -> Right -> Parent = Root;
	
	NewRoot -> Left = Root;
	
	__AdjustHeight ( Root );
	__AdjustHeight ( NewRoot );
	
	return NewRoot;
	
};


MT::Timing::TaskSleep :: SleepInfo * MT::Timing::TaskSleep :: __Balance ( SleepInfo * Root )
{
	
	int32_t Balance = 0;
	int32_t SubBalance = 0;
	
	Balance += ( Root -> Left != NULL ) ? Root -> Left -> Height : - 1;
	Balance -= ( Root -> Right != NULL ) ? Root -> Right -> Height : - 1;
	
	if ( Balance > 1 )
	{
		
		SubBalance += ( Root -> Left -> Left != NULL ) ? Root -> Left -> Left -> Height : - 1;
		SubBalance -= ( Root -> Left -> Right != NULL ) ? Root -> Left -> Right -> Height : - 1;
		
		if ( SubBalance > 0 )
		{
			
			Root = __RotateRight ( Root );
			
		}
		else
		{
			
			__RotateLeft ( Root -> Left );
			Root = __RotateRight ( Root );
			
		}
		
	}
	else if ( Balance < - 1 )
	{
		
		SubBalance -= ( Root -> Right -> Left != NULL ) ? Root -> Right -> Left -> Height : - 1;
		SubBalance += ( Root -> Right -> Right != NULL ) ? Root -> Right -> Right -> Height : - 1;
		
		if ( SubBalance > 0 )
		{
			
			Root = __RotateLeft ( Root );
			
		}
		else
		{
			
			__RotateRight ( Root -> Right );
			Root = __RotateLeft ( Root );
			
		}
		
	}
	
	return Root;
	
};


