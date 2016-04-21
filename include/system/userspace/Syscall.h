#ifndef SYSTEM_USERSPACE_SYSCALL_H
#define SYSTEM_USERSPACE_SYSCALL_H

#include <system/System.h>
#include <system/userspace/UserSpace.h>

#include <interrupt/InterruptHandlers.h>

#include <mt/tasking/Task.h>

namespace System
{
	
	namespace UserSpace
	{
		
		class Syscall
		{
		public:
			
			static const uint32_t kNumSyscalls = 0x1;
			
			static const uint32_t kSyscall_Exit = 0x0;
			
			static void Init ();
			
		private:
			
			static const uint32_t kSysCallInterrupt_Vector = 0x80;
			
			static void CallInterruptHandler ( Interrupt::InterruptHandlers :: ISRFrame * Frame );
			
			static void ( * Handlers [ kNumSyscalls ] ) ( Interrupt::InterruptHandlers :: ISRFrame * Frame, volatile MT::Tasking :: Task * CurrentTask );
			
			static void DummySyscall ( Interrupt::InterruptHandlers :: ISRFrame * Frame, volatile MT::Tasking :: Task * CurrentTask );
			
		};
		
	};
	
};

#endif
