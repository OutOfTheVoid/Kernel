#include <mt/timing/TaskSleep.h>

#include <mm/KMalloc.h>

#include <hw/cpu/Processor.h>
#include <hw/cpu/TSC.h>

#include <interrupt/IState.h>

#include <interrupt/APIC.h>

MT::Timing::TaskSleep :: SleepInfo * MT::Timing::TaskSleep :: TreeRoot = NULL;

uint64_t MT::Timing::TaskSleep :: NextInterrupt;

MT::Timing::TaskSleep :: TimingSource MT::Timing::TaskSleep :: Source;

MT::Timing::TaskSleep :: TimingControl_Union MT::Timing::TaskSleep :: TimingControl;

void MT::Timing::TaskSleep :: Init ()
{
	
	Source = kTimingSource_PIT;
	
	TimingControl.PITControl.TSCAtInterrupt = ::HW::CPU::TSC :: Read ();
	
};

uint64_t MT::Timing::TaskSleep ::  GetCurrentTimeNS ()
{
	
	switch ( Source )
	{
	
	case kTimingSource_PIT:
			return static_cast <uint64_t> ( ::HW::CPU::TSC :: Read () / ( Interrupt::APIC :: GetBusFrequencey () / 1000000000.0 ) );
		break;
		
	default:
		
		return 0;
		
	}
	
};

inline uint64_t MT::Timing::TaskSleep :: DelayMSToNS ( double MS )
{
	
	return static_cast <uint64_t> ( MS * 1000000.0 );
	
};

void MT::Timing::TaskSleep :: SetNextInterrupt ( uint64_t ThresholdNS )
{
	
	
	
};

void MT::Timing::TaskSleep :: SleepCurrent ( double Milliseconds )
{
	
	if ( Milliseconds >= 0.0 )
		return;
	
	uint64_t Current = GetCurrentTimeNS ();
	uint64_t ThresholdNS = Current + DelayMSToNS ( Milliseconds );
	
	::HW::CPU::Processor :: CPUInfo * ThisCPU = ::HW::CPU::Processor :: GetCurrent ();
	
	bool ReInt = Interrupt::IState :: ReadAndSetBlock ();
	
	Tasking::Task :: Task_t * PendingTask = ThisCPU -> CurrentTask;
	PendingTask -> State = Tasking::Task :: kState_Blocked;
	
	Interrupt::IState :: WriteBlock ( ReInt );
	
	SleepInfo * NewNode = CreateNode ( ThisCPU -> CurrentTask, ThresholdNS );
	
	ReInt = Interrupt::IState :: ReadAndSetBlock ();
	
	if ( NextInterrupt > ThresholdNS )
		SetNextInterrupt ( ThresholdNS );
	
	InsertNode ( NewNode );
	
	Interrupt::IState :: WriteBlock ( ReInt );
	
	__asm__ volatile ( "int 0x20" );
	
};

void MT::Timing::TaskSleep :: OnInterrupt ()
{
	
	
	
};

MT::Timing::TaskSleep :: SleepInfo * MT::Timing::TaskSleep :: CreateNode ( Tasking::Task :: Task_t * PendingTask, uint64_t ThresholdNS )
{
	
	SleepInfo * New = reinterpret_cast <SleepInfo *> ( mm_kmalloc ( sizeof ( SleepInfo ) ) );
	
	if ( New == NULL )
		return NULL;
	
	New -> PendingTask = PendingTask;
	New -> ThresholdNS = ThresholdNS;
	
	New -> Parent = NULL;
	New -> Left = NULL;
	New -> Right = NULL;
	
	New -> Height = 0;
	
	return New;
	
};

void MT::Timing::TaskSleep :: DestroyNode ( SleepInfo * Node )
{
	
	mm_kfree ( Node );
	
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
		
		if ( Node -> ThresholdNS > Current -> ThresholdNS )
		{
			
			if ( Current -> Right == NULL )
			{
				
				Node -> Parent = Current;
				Current -> Right = Node;
				
				break;
				
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
					( TreeRoot ) -> Right = NULL;
					
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
				Node -> Left = NULL;
				
				return;
				
			}
			
		}
		else
		{
			
			if ( Node -> Right != NULL )
			{
				
				TreeRoot = Node -> Right;
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


inline MT::Timing::TaskSleep :: SleepInfo * MT::Timing::TaskSleep :: __Balance ( SleepInfo * Root )
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


