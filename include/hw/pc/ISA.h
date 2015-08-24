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
			
			static void InitIOInterrupts ();
			
			static void SetIRQHandler ( uint8_t IRQ, void ( * Handler ) ( Interrupt::InterruptHandlers :: ISRFrame * ) );
			static void SetIRQEnabled ( uint8_t IRQ, bool Enabled );

			static bool TryAllocateIRQ ( uint8_t IRQ );
			static void FreeIRQ ( uint8_t );
			
		private:
			
			static const uint8_t kIRQ_InterruptBase = 0x40;
			static const uint8_t kIRQ_GlobalSystemInterruptBase = 0x00;
			
			static const uint8_t kMADT_ISABusNumber = 0x00;
			
		};
		
	};
	
};

#endif
