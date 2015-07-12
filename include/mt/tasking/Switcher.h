#ifndef MT_TASKING_SWITCHER_H
#define MT_TASKING_SWITCHER_H

#include <mt/tasking/Tasking.h>
#include <mt/tasking/Task.h>

namespace MT
{
	
	namespace Tasking
	{
		
		class Switcher
		{
		public:
			
			static void SwitchTo ( Task :: Task_t * NewTask, Task :: Task_t * OldTask );
			
		};
		
	};
	
};

#endif
