#ifndef MT_TASKING_TASK_H
#define MT_TASKING_TASK_H

#include <mt/tasking/Tasking.h>

#include <interrupt/InterruptHandlers.h>

#include <cpputil/Linkage.h>

#include <hw/cpu/Math.h>

// Forward declare those classes we can't include for odd reasons
namespace MM
{

	namespace Paging
	{

		class AddressSpace;
		class PageTable;

	};

};

namespace MT
{
	
	namespace Tasking
	{
		
		class Task
		{
		public:
			
			static const uint32_t kFlag_User = 0x0000;
			static const uint32_t kFlag_Kernel = 0x0001;
			static const uint32_t kFlag_Scheduler_Yield = 0x0002;
			static const uint32_t kFlag_Math = 0x0004;
			static const uint32_t kFlag_MathInit = 0x0008;
			static const uint32_t kFlag_Suspended = 0x0010;
			static const uint32_t kFlag_CPUInitStack = 0x0020;
			
			static const uint32_t kPrivelege_Exec = 0x0001;
			static const uint32_t kPrivelege_IO = 0x0002;
			
			static const uint32_t kState_Runnable = 0x0001;
			static const uint32_t kState_Blocked = 0x0002;
			static const uint32_t kState_KernelSelectable = 0x0004;
			static const uint32_t kState_Dead = 0x0008;
			static const uint32_t kState_Idle = 0x0010;
			
			typedef struct Task_Struct
			{
				
				uint32_t ID;
				char Name [ 256 ];
				
				uint32_t Flags;
				uint32_t State;
				
				void * KStackBottom;
				void * KStack;
				uint32_t KSS;
				
				::HW::CPU::Math :: MathState Math;
				
				uint32_t User;
				
				uint32_t Privelege;
				
				uint32_t Priority;
				uint32_t RunOut;
				
				struct Task_Struct * Next;
				struct Task_Struct * Previous;
				
				MM::Paging::PageTable * MemoryMapping;
				MM::Paging::AddressSpace * MemorySpace;
				
			} Task_t;
			
			static Task_t * CreateKernelTask ( const char * Name, void * Entry, uint32_t StackSize, uint32_t Priority );
			
			static void DestroyKernelTask ( Task_t * ToDestroy );
			
		private:
			
			static void StackPush ( void ** Stack, uint32_t Value );
			
		};
		
	};
	
};

#endif