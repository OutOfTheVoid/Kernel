#ifndef MT_TASKING_SWITCHER_H
#define MT_TASKING_SWITCHER_H

#include <mt/tasking/Tasking.h>
#include <mt/tasking/Task.h>

#include <mt/Synchronization/Spinlock.h>

namespace MT
{
	
	namespace Tasking
	{
		
		class Switcher
		{
		public:
			
			static void Init ( Synchronization::Spinlock :: Spinlock_t * TaskTableLock );
			static void SwitchTo ( volatile Task * NewTask, volatile Task * OldTask );
			
		};
		
	};
	
};

#endif
