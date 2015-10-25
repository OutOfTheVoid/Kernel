#ifndef MM_PAGING_INVALIDATION_H
#define MM_PAGING_INVALIDATION_H

#include <mm/paging/PageTable.h>

#include <interrupt/InterruptHandlers.h>

#include <cpputil/Linkage.h>

ASM_LINKAGE void hw_cpu_exceptionPageFault ( Interrupt::InterruptHandlers :: ISRFrame * Frame );

namespace MM
{
	
	namespace Paging
	{
		
		class Invalidation
		{
		public:	
			
			static void InitInterrupt ();
			
			static void InvalidatePages ( uint32_t CR3, uint32_t PageAddress, uint32_t Count = 1 );
			
		private:
			
			friend void ::hw_cpu_exceptionPageFault ( Interrupt::InterruptHandlers :: ISRFrame * Frame );
			
			static const uint32_t kPageFaultErrorCodeFlag_Protection = 0x00000001;
			static const uint32_t kPageFaultErrorCodeFlag_Read = 0x00000000;
			static const uint32_t kPageFaultErrorCodeFlag_Write = 0x00000002;
			static const uint32_t kPageFaultErrorCodeFlag_Supervisor = 0x00000000;
			static const uint32_t kPageFaultErrorCodeFlag_User = 0x00000004;
			
			static const uint8_t kVector_InvalidateIPI = 0x21;
			
			static uint32_t CR3;
			
			static uint32_t VirtualStart;
			static uint32_t PageCount;
			
			static uint32_t ProccessorCount;
			
			static MT::Synchronization::Spinlock :: Spinlock_t InvalLock;
			
			static bool PageFault ( uint32_t ErrorCode );
			
			static inline uint32_t GetCR2 ()
			{
				
				register uint32_t Value;
				__asm__ volatile ( "mov %0, cr2" : "=r" ( Value ) );
				return Value;
				
			}
			
			static void InvalidationHandler ( Interrupt::InterruptHandlers :: ISRFrame * Frame );
			
		};
		
	};
	
};

#endif
