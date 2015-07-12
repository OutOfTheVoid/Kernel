#ifndef INTERRUPTS_INTERRUPTHANDLERS_H
#define INTERRUPTS_INTERRUPTHANDLERS_H

#include <stdint.h>
#include <stddef.h>

#include <interrupt/interrupt.h>

#include <mt/hw/TSS.h>

namespace Interrupt
{
	
	class InterruptHandlers
	{
	public:
		
		static void Init ();
		static void APInit ();
		
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
			
			uint32_t InterruptNumber;
			uint32_t ErrorCode;
			
			uint32_t EIP;
			uint32_t CS;
			uint32_t EFlags;
			uint32_t UserESP;
			uint32_t SS;
			
		} __attribute__((packed)) ISRFrame;
		
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
			
			uint32_t InterruptNumber;
			uint32_t IRQNumber;
			
			uint32_t EIP;
			uint32_t CS;
			uint32_t EFlags;
			uint32_t UserESP;
			uint32_t SS;
			
		} __attribute__((packed)) IRQFrame;
		
		static void SetInterruptHandler ( uint32_t InterruptNumber, void ( * Handler ) ( Interrupt::InterruptHandlers :: ISRFrame * ) = NULL );
		static void SetCPInterruptKernelStack ( void * StackTop );
		
	private:
		
		static void InstallSystemInterruptHandlers ();
		
		MT::HW::TSS :: TSS_t CPInterruptTSS;
		
	};
	
};

#endif
