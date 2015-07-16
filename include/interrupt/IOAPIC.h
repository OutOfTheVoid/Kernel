#ifndef INTERRUPT_IOAPIC_H
#define INTERRUPT_IOAPIC_H

#include <interrupt/Interrupt.h>

#include <stdint.h>

#include <util/Vector.h>

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
		
		static void DefineRedirectionEntry ( uint32_t IRQ, uint32_t TargetVector, DeliveryMode Mode, bool ActiveHigh, bool EdgeTriggered, bool InitiallyMasked );
		
	private:
		
		typedef struct
		{
			
			void * BaseAddress;
			
			uint8_t ID;
			uint32_t GlobalSystemInterruptBase;
			
		} IOAPICInfo;
		
		static const uint32_t kRegister_ID = 0x00;
		static const uint32_t kRegister_Version = 0x01;
		static const uint32_t kRegister_ArbitraionPriority = 0x02;
		static const uint32_t kRegister_Flags = 0x03;
		
		static const uint32_t kRegister_BaseRedirectionEntry = 0x10;
		
		static const uint32_t kFlags_Usable = 0x00000001;
		
		static Vector <IOAPICInfo> * IOAPICs;
		
		static uint32_t ReadRegister ( void * BaseAddress, uint32_t Register );
		static void WriteRegister ( void * BaseAddress, uint32_t Register, uint32_t Value );
		
		
	};
	
};

#endif
