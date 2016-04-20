#ifndef MT_TIMING_TASKSLEEP_H
#define MT_TIMING_TASKSLEEP_H

#include <mt/timing/Timing.h>

#include <mt/tasking/Task.h>

#include <mt/synchronization/Spinlock.h>

#include <stdint.h>

ASM_LINKAGE void mt_timing_tasksleep_pitirqhandler ( Interrupt::InterruptHandlers :: ISRFrame * Frame );

namespace MT
{
	
	namespace Timing
	{
		
		class TaskSleep
		{
		public:
			
			static void Init ();
			static void SleepCurrent ( double Milliseconds );
			
		private:
			
			friend void ::mt_timing_tasksleep_pitirqhandler ( Interrupt::InterruptHandlers :: ISRFrame * Frame );
			
			typedef struct SleepInfo_Struct
			{
				
				volatile Tasking :: Task * PendingTask;
				
				uint64_t ThresholdNS;
				
				int32_t Height;
				
				struct SleepInfo_Struct * Parent;
				struct SleepInfo_Struct * Left;
				struct SleepInfo_Struct * Right;
				
			} SleepInfo;
			
			typedef enum
			{
				
				kTimingSource_TSC,
				kTimingSource_PMTimer,
				kTimingSource_None
				
			} TimingSource;
			
			typedef enum
			{
				
				kInterruptSource_PIT,
				kInterruptSource_HPET,
				kInterruptSource_None
				
			} InterruptSource;
			
			static const uint32_t kMaxFreeCount = 200;
			
			static Synchronization::Spinlock :: Spinlock_t TimerLock;
			static Synchronization::Spinlock :: Spinlock_t TreeLock;
			static SleepInfo * TreeRoot;
			
			static Synchronization::Spinlock :: Spinlock_t FreeListLock;
			static SleepInfo * FreeHead;
			static uint32_t FreeCount;
			
			static uint64_t NextInterrupt;
			
			static TimingSource TimeSource;
			static InterruptSource IntSource;
			
			static uint64_t GetCurrentTimeNS ();
			static inline uint64_t DelayMSToNS ( double MS );
			
			static void SetNextInterrupt ( uint64_t ThresholdNS );
			
			static void OnInterrupt ();
			
			static SleepInfo * CreateNode ();
			static void DestroyNode ( SleepInfo * Node );
			
			static void InsertNode ( SleepInfo * Node );
			static void DeleteNode ( SleepInfo * Node );
			
			static SleepInfo * __Balance ( SleepInfo * Root );
			
			static inline int32_t __AdjustHeight ( SleepInfo * Node );
			
			static inline SleepInfo * __RotateLeft ( SleepInfo * Root );
			static inline SleepInfo * __RotateRight ( SleepInfo * Root );
			
			static inline SleepInfo * __LeftMostNode ( SleepInfo * Node );
			
		};
		
	};
	
};

#endif
