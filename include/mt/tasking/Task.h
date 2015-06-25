#ifndef MT_TASKING_TASK_H
#define MT_TASKING_TASK_H

#include <mt/tasking/Tasking.h>

#include <interrupt/InterruptHandlers.h>

#include <mm/paging/AddressSpace.h>

namespace MT
{
	
	namespace Tasking
	{
		
		class Task
		{
		public:
			
			static const uint32_t kFlag_Kernel = 0x0001;
			
			static const uint32_t kPrivelege_Exec = 0x0001;
			static const uint32_t kPrivelege_IO = 0x0002;
			
			static const uint32_t kState_Runnable = 0x0001;
			static const uint32_t kState_Sleeping = 0x0002;
			static const uint32_t kState_KernelSelectable = 0x0004;
			static const uint32_t kState_Dead = 0x0008;
			static const uint32_t kState_Idle = 0x0010;
			
			typedef struct Task_Struct
			{
				
				uint32_t ID;
				char Name [ 256 ];
				
				uint32_t Flags;
				uint32_t State;
				
				void * WaitAttribute;
				
				Interrupt::InterruptHandlers :: ISRFrame PState;
				
				uint32_t User;
				
				uint32_t Privelege;
				
				uint32_t Priority;
				
				struct Task_Struct * Next;
				struct Task_Struct * Previous;
				
				MM::Paging::AddressSpace * AddressSpace;
				
			} Task_t;
			
			Task_t * NewTask (  );
			
		private:
			
			
		};
		
	};
	
};

#endif