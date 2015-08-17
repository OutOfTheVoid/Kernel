#ifndef MM_PAGING_INVALIDATION_H
#define MM_PAGING_INVALIDATION_H

#include <mm/paging/PageTable.h>

#include <interrupt/InterruptHandlers.h>

namespace MM
{
	
	namespace Paging
	{
		
		class Invalidation
		{
		public:	
			
			static void PInit ();
			
			static void InvalidatePage ( uint32_t CR3, uint32_t PageAddress );
			
		private:
			
			static const uint32_t kPageFaultErrorCodeFlag_Protection = 0x00000001;
			static const uint32_t kPageFaultErrorCodeFlag_Read = 0x00000000;
			static const uint32_t kPageFaultErrorCodeFlag_Write = 0x00000002;
			static const uint32_t kPageFaultErrorCodeFlag_Supervisor = 0x00000000;
			static const uint32_t kPageFaultErrorCodeFlag_User = 0x00000004;
			
			static bool PageFault ( uint32_t ErrorCode );
			
			static inline uint32_t GetCR2 ()
			{
				
				register uint32_t Value;
				__asm__ volatile ( "mov %0, cr2" : "=r" ( Value ) );
				
				return Value;
				
			}
			
		};
		
	};
	
};

#endif
