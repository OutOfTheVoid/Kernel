#ifndef MT_TIMING_TASKSLEEP_H
#define MT_TIMING_TASKSLEEP_H

#include <mt/timing/Timing.h>

#include <mt/tasking/Task.h>

#include <mt/synchronization/Spinlock.h>

#include <stdint.h>

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
			
			typedef struct SleepInfo_Struct
			{
				
				Tasking::Task :: Task_t * PendingTask;
				
				uint64_t ThresholdNS;
				
				int32_t Height;
				
				struct SleepInfo_Struct * Parent;
				struct SleepInfo_Struct * Left;
				struct SleepInfo_Struct * Right;
				
			} SleepInfo;
			
			typedef enum
			{
				
				kTimingSource_PIT,
				kTimingSource_HPET,
				kTimingSource_None
				
			} TimingSource;
			
			typedef struct
			{
				
				uint64_t TSCAtInterrupt;
				
			} PITControlInfo;
			
			static SleepInfo * TreeRoot;
			
			static uint64_t NextInterrupt;
			
			static union TimingControl_Union
			{
				
				PITControlInfo PITControl;
				
			} TimingControl;
			
			static TimingSource Source;
			
			static uint64_t GetCurrentTimeNS ();
			static inline uint64_t DelayMSToNS ( double MS );
			
			static void SetNextInterrupt ( uint64_t ThresholdNS );
			
			static void OnInterrupt ();
			
			static SleepInfo * CreateNode ( Tasking::Task :: Task_t * PendingTask, uint64_t ThresholdNS );
			static void DestroyNode ( SleepInfo * Node );
			
			static void InsertNode ( SleepInfo * Node );
			static void DeleteNode ( SleepInfo * Node );
			
			static inline int32_t __AdjustHeight ( SleepInfo * Node );
			
			static inline SleepInfo * __Balance ( SleepInfo * Root );
			
			static inline SleepInfo * __RotateLeft ( SleepInfo * Root );
			static inline SleepInfo * __RotateRight ( SleepInfo * Root );
			
			static inline SleepInfo * __LeftMostNode ( SleepInfo * Node );
			
		};
		
	};
	
};

#endif
