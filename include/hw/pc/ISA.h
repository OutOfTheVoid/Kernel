#ifndef HW_PC_ISA
#define HW_PC_ISA

#include <hw/pc/ISA.h>

#include <stdint.h>

#include <interrupt/InterruptHandlers.h>

namespace HW
{
	
	namespace PC
	{
		
		class ISA
		{
		public:
			
			static void SetIRQHandler ( uint8_t IRQ, void ( * Handler ) ( Interrupt::InterruptHandlers :: ISRFrame * ) );
			
		private:
			
			static const uint8_t kIRQ_InterruptBase = 0x40;
			
		};
		
	};
	
};

#endif
