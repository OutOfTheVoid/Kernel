#ifndef HW_CPU_MSR_H
#define HW_CPU_MSR_H

#include <hw/HW.h>
#include <hw/cpu/CPU.h>

namespace HW
{
	
	namespace CPU
	{
		
		class MSR
		{
		public:
			
			static const uint32_t kMSR_APIC_BASE = 0x1B;
			static const uint32_t kMSR_APIC_BSP = 0x100;
			static const uint32_t kMSR_APIC_ENABLE = 0x800;
			
			static inline void GetMSR ( uint32_t Register, uint32_t * EAX, uint32_t * EDX )
			{
				
				__asm__ volatile ( "rdmsr" : "=a" ( * EAX ), "=d" ( * EDX ) : "c" ( Register ) );
				
			};
			
			static inline void SetMSR ( uint32_t Register, uint32_t * EAX, uint32_t * EDX )
			{
				
				__asm__ volatile ( "wrmsr" :: "a" ( EAX ), "d" ( EDX ), "c" ( Register ) );
				
			};
			
		private:
			
		};
		
	};
	
};

#endif
