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
			
			static void Init ();
			
			static void RegisterSyscallHandler ( void ( * Handler ) ( Interrupt::InterruptHandlers :: ISRFrame * Frame, MT::Tasking::Task :: Task_t * Caller ) );
			
		private:
			
			static const uint32_t kSysCallInterrupt_Vector = 0x100;
			
			static void CallHandler ( Interrupt::InterruptHandlers :: ISRFrame * Frame );
			
		};
		
	};
	
};

#endif
