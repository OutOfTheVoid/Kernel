#ifndef MT_TASKING_SCHEDULER_H
#define MT_TASKING_SCHEDULER_H

#include <mt/tasking/Tasking.h>
#include <mt/tasking/Task.h>
#include <mt/synchronization/Spinlock.h>

#include <hw/cpu/Processor.h>

namespace MT
{
	
	namespace Tasking
	{
		
		class Scheduler
		{
		public:
			
			static const double kSchedulingQuantumMS = 10.0;
			
			static void Init ();
			static void PInit ( uint32_t Priority );
			
			static void Schedule ();
			
			static void AddTask ( Task :: Task_t * ToAdd );
			static void AddTaskInternal ( volatile Task :: Task_t * ToAdd );
			
			static void SuspendTask ( Task :: Task_t * ToSuspend );
			static void SuspendCurrentTask ();
			
			static void KillTask ( Task :: Task_t * ToKill );
			static void KillCurrentTask ();
			
			static void YieldCurrentTask ();
			
			static Task :: Task_t * GetNextDeadTask ();
			
			static inline void Preemt ( Synchronization::Spinlock :: Spinlock_t * ReleaseOnSchedule = NULL )
			{
				
				if ( ReleaseOnSchedule != NULL )
					::HW::CPU::Processor :: GetCurrent () -> ReleaseOnSchedule = ReleaseOnSchedule;
				
				__asm__ volatile ( "int 0x20" );
				
			};
			
		private:
			
			friend class Task;
			
			static volatile Task :: Task_t * TaskTable [ 0x20 ];
			static Synchronization::Spinlock :: Spinlock_t TaskTableLock;
			
			static volatile Task :: Task_t * ReapingListHead;
			static Synchronization::Spinlock :: Spinlock_t ReapListLock;
			
			static Synchronization::Spinlock :: Spinlock_t TaskIDLock;
			static uint64_t MaxID;
			
			static uint64_t GetNewTaskID ();
			
		};
		
	};
	
};

#endif
