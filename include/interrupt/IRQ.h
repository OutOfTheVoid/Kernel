#ifndef INTERRUPT_IRQ_H
#define INTERRUPT_IRQ_H

#include <interrupt/Interrupt.h>
#include <interrupt/InterruptHandlers.h>
#include <interrupt/PIC.h>
#include <interrupt/APIC.h>

#include <mt/synchronization/Spinlock.h>

namespace Interrupt
{
	
	class IRQ
	{
	public:
		
		static const uint8_t kIRQ_BaseInterrupt = 0x40;
		
		static void Init ();
		
		static void SetIRQ ( uint8_t IRQ, void ( * Handler ) ( InterruptHandlers :: IRQFrame * ) );
		static void CallIRQ ( InterruptHandlers :: IRQFrame * );
		
		static void EndIRQ ( uint8_t Number );
		
	private:
		
		friend class Interrupt::APIC;
		friend class Interrupt::PIC;
		
		typedef enum
		{
			
			kIRQExit_PIC = 0,
			kIRQExit_APIC = 1,
			kIRQExit_None = 0xFFFFFFFF
			
		} IRQExit;
		
		static IRQExit ExitMode;
		static uint8_t MaxIRQ;
		
		static void ( * IRQHandlers [ 0x20 ] ) ( InterruptHandlers :: IRQFrame * );
		
		static MT::Synchronization::Spinlock :: Spinlock_t ListLocks [ 0x20 ];
		
	};
	
};

#endif
