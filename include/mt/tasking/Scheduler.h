#ifndef MT_TASKING_SCHEDULER_H
#define MT_TASKING_SCHEDULER_H

#include <mt/tasking/Tasking.h>
#include <mt/tasking/Task.h>
#include <mt/synchronization/Spinlock.h>

namespace MT
{
	
	namespace Tasking
	{
		
		class Scheduler
		{
		public:
			
			static const double kSchedulingQuantumMS = 10.0;
			
			static void Init ();
			
			static void PInit ();
			static void Schedule ();
			
			static void AddTask ( Task :: Task_t * ToAdd );
			
		private:
			
			friend class Task;
			
			static Task :: Task_t * TaskTable [ 0x20 ];
			static Synchronization::Spinlock :: Spinlock_t TTLock;
			
			static Synchronization::Spinlock :: Spinlock_t TIDLock;
			static uint64_t MaxID;
			
			static uint64_t GetNewTaskID ();
			
		};
		
	};
	
};

#endif
