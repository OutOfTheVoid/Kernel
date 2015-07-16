#ifndef INTERRUPT_IOAPIC_H
#define INTERRUPT_IOAPIC_H

#include <interrupt/Interrupt.h>

#include <stdint.h>

namespace Interrupt
{
	
	class IOAPIC
	{
	public:	
		
		typedef enum
		{
			
			kDeliveryMode_Fixed,
			kDeliveryMode_LowPriority,
			kDeliveryMode_SMI,
			kDeliveryMode_NMI,
			kDeliveryMode_Init,
			kDeliveryMode_ExtInt
			
		} DeliveryMode;
		
		
		
		static void Init ();
		
		static void DefineRedirectionEntry ( uint32_t IRQ, uint32_t TargetVector, DeliveryMode Mode, bool ActiveHigh, bool EdgeTriggered, bool InitiallyMasked,  );
		
	private:
		
		kRegister_ID = 0x00;
		kRegister_Version = 0x01;
		kRegister_ArbitraionPriority = 0x02;
		kRegister_Flags = 0x03;
		
		kRegister_BaseRedirectionEntry = 0x10;
		
		kFlags_Usable = 0x00000001;
		
		static uint32_t ReadRegister ( void * BaseAddress, uint32_t Register );
		static void WriteRegister ( void * BaseAddress, uint32_t Register, uint32_t Value );
		
		static uint32_t Count;
		static uint32_t ** BaseAddresses;
		
	};
	
};

#endif
