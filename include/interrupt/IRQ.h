#ifndef INTERRUPT_IRQ_H
#define INTERRUPT_IRQ_H

#include <interrupt/Interrupt.h>
#include <interrupt/PIC.h>
#include <interrupt/APIC.h>

namespace Interrupt
{
	
	class IRQ
	{
	public:
		
		typedef struct
		{
			
			uint32_t DS;
			
			uint32_t EDI;
			uint32_t ESI;
			
			uint32_t EBP;
			uint32_t ESP;
			
			uint32_t EBX;
			uint32_t EDX;
			uint32_t ECX;
			uint32_t EAX;
			
			uint32_t IRQNumber;
			
			uint32_t EIP;
			uint32_t CS;
			uint32_t EFlags;
			uint32_t UserESP;
			uint32_t SS;
			
		} IRQFrame;
		
		static const uint8_t kIRQ_BaseInterrupt = 0x30;
		
		static void Init ();
		
		static void SetIRQ ( uint8_t IRQ, void ( * Handler ) ( IRQFrame * ) );
		static void CallIRQ ( IRQFrame * );
		
		static void EndIRQ ( uint8_t Number );
		
	private:
		
		friend class Interrupt::APIC;
		friend class Interrupt::PIC;
		
		typedef enum
		{
			
			kIRQExit_PIC = 0,
			kIRQExit_None = 1
			
		} IRQExit;
		
		static IRQExit ExitMode;
		static uint8_t MaxIRQ;
		
		static void ( * IRQHandlers [ 0x20 ] ) ( IRQFrame * );
		
	};
	
};

#endif
