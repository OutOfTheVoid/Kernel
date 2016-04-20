#ifndef MT_TASKING_TASK_H
#define MT_TASKING_TASK_H

#include <mt/tasking/Tasking.h>

#include <mt/process/UserProcess.h>

#include <interrupt/InterruptHandlers.h>

#include <cpputil/Linkage.h>

#include <hw/cpu/Math.h>

#include <mm/paging/AddressSpace.h>
#include <mm/paging/PageTable.h>

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
			
			static const uint32_t kPrivelege_None = 0x0001;
			static const uint32_t kPrivelege_Exec = 0x0001;
			static const uint32_t kPrivelege_IO = 0x0002;
			
			static const uint32_t kState_Runnable = 0x0001;
			static const uint32_t kState_Blocked = 0x0002;
			static const uint32_t kState_KernelSelectable = 0x0004;
			static const uint32_t kState_Dead = 0x0008;
			static const uint32_t kState_Idle = 0x0010;
			static const uint32_t kState_RunnableYielded = 0x0020;
			
			static const uint32_t kPriority_RealTime_Min = 4;
			static const uint32_t kPriority_RealTime_Max = 0;
			
			static const uint32_t kPriority_System_Min = 9;
			static const uint32_t kPriority_System_Max = 5;
			
			static const uint32_t kPriority_HighUser_Min = 14;
			static const uint32_t kPriority_HighUser_Max = 10;
			static const uint32_t kPriority_LowUser_Min = 19;
			static const uint32_t kPriority_LowUser_Max = 15;
			
			uint64_t ID;
			uint32_t ThreadID;
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
			
			uint32_t MaxPriority;
			uint32_t MinPriority;
			
			volatile Task * Next;
			volatile Task * Previous;
			
			MM::Paging::PageTable * MemoryMapping;
			MM::Paging::AddressSpace * MemorySpace;
			
			Process :: UserProcess * Process;
			
			static Task * CreateKernelTask ( const char * Name, void * Entry, uint32_t StackSize, uint32_t MaxPriority, uint32_t MinPriority );
			static Task * CreateUserTask ( const char * Name, MT::Process :: UserProcess * ContainerProcess, void * Entry, uint32_t StackSize, uint32_t MaxPriority, uint32_t MinPriority );
			
			static void DestroyKernelTask ( Task * ToDestroy );
			
		private:
			
			static void StackPush ( void ** Stack, uint32_t Value );
			
		};
		
	};
	
};

#endif