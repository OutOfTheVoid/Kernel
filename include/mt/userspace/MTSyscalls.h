#ifndef MT_USERSPACE_SYSCALLS_H
#define MT_USERSPACE_SYSCALLS_H

#include <mt/userspace/UserSpace.h>

#include <interrupt/InterruptHandlers.h>

#include <mt/tasking/Task.h>

namespace MT
{
	
	namespace UserSpace
	{
		
		class MTSyscalls
		{
		public:
			
			static void TaskExit ( Interrupt::InterruptHandlers :: ISRFrame * Frame, volatile MT::Tasking :: Task * CurrentTask );
			
		};
		
	};
	
};

#endif
