#ifndef MT_SYNCHRONIZATION_RECURSIVESPINLOCK_H
#define MT_SYNCHRONIZATION_RECURSIVESPINLOCK_H

#include <mt/synchronization/Synchronization.h>
#include <mt/synchronization/Spinlock.h>

#include <mt/tasking/Task.h>

#include <stdint.h>

namespace MT
{
	
	namespace Synchronization
	{
		
		class RecursiveSpinlock
		{
		public:
			
			typedef struct
			{
				
				Spinlock :: Spinlock_t Lock;
				
				
				
			} RecursiveSpinlock_t;
			
		};
		
	};
	
};

#endif
