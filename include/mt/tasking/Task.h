#ifndef MT_TASKING_TASK_H
#define MT_TASKING_TASK_H

#include <mt/tasking/Tasking.h>

#include <interrupt/InterruptHandlers.h>

#include <mm/paging/PageTable.h>
#include <mm/paging/AddressSpace.h>

namespace MT
{
	
	namespace Tasking
	{
		
		class Task
		{
		public:
			
			static const uint32_t kFlag_User = 0x0000;
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
				
				void * KStack;
				uint32_t KSS;
				
				Interrupt::InterruptHandlers :: ISRFrame * PState;
				
				uint32_t User;
				
				uint32_t Privelege;
				
				uint32_t Priority;
				uint32_t RunOut;
				
				struct Task_Struct * Next;
				struct Task_Struct * Previous;
				
				MM::Paging::PageTable * MemoryMapping;
				MM::Paging::AddressSpace * MemorySpace;
				
			} Task_t;
			
		private:
			
			
		};
		
	};
	
};

#endif