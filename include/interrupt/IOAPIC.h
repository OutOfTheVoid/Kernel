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
		
		static void Init ();
		
		static void DefineFixedRedirectionEntry ( uint32_t Interrupt, uint32_t TargetVector, uint8_t APICID, bool ActiveHigh, bool EdgeTriggered, bool InitiallyMasked );
		static void DefineLowestPriorityRedirectionEntry ( uint32_t Interrupt, uint32_t TargetVector, bool ActiveHigh, bool EdgeTriggered, bool InitiallyMasked );
		
		static void SetRedirectionEntryEnabled ( uint32_t Interrupt, bool Enabled );
		
	private:
		
		typedef struct
		{
			
			void * BaseAddress;
			
			uint8_t ID;
			uint8_t Version;
			
			uint32_t GlobalSystemInterruptBase;
			uint32_t GlobalSystemInterruptCount;
			
		} IOAPICInfo;
		
		static const uint32_t kRegister_ID = 0x00;
		static const uint32_t kRegister_Version = 0x01;
		static const uint32_t kRegister_ArbitraionPriority = 0x02;
		static const uint32_t kRegister_Flags = 0x03;
		
		static const uint32_t kRegister_BaseRedirectionEntry = 0x10;
		
		static const uint32_t kFlags_Usable = 0x00000001;
		
		static const uint32_t kRedirectionEntry_Low_Deliveryode_Fixed = 0x00000000;
		static const uint32_t kRedirectionEntry_Low_Deliveryode_LowestPriority = 0x00000100;
		static const uint32_t kRedirectionEntry_Low_Deliveryode_SMI = 0x00000200;
		static const uint32_t kRedirectionEntry_Low_Deliveryode_NMI = 0x00000400;
		static const uint32_t kRedirectionEntry_Low_Deliveryode_INIT = 0x00000500;
		static const uint32_t kRedirectionEntry_Low_Deliveryode_ExtINT = 0x00000700;
		
		static const uint32_t kRedirectionEntry_Low_DestinationMode_Physical = 0x00000000;
		static const uint32_t kRedirectionEntry_Low_DestinationMode_Logical = 0x00000800;
		
		static const uint32_t kRedirectionEntry_Low_DeliveryStatus_Delivered = 0x00000000;
		static const uint32_t kRedirectionEntry_Low_DeliveryStatus_Pending = 0x00001000;
		
		static const uint32_t kRedirectionEntry_Low_PinPolarity_ActiveHigh = 0x000000000;
		static const uint32_t kRedirectionEntry_Low_PinPolarity_ActiveLow = 0x000002000;
		
		static const uint32_t kRedirectionEntry_Low_TriggerMode_Edge = 0x00000000;
		static const uint32_t kRedirectionEntry_Low_TriggerMode_Level = 0x00008000;
		
		static const uint32_t kRedirectionEntry_Low_Mask_Clear = 0x00000000;
		static const uint32_t kRedirectionEntry_Low_Mask_Set = 0x00010000;
		
		static const uint32_t kRedirectionEntry_High_BitBase_PhysicalDestination = 24;
		
		static const uint32_t kRedirectionEntry_High_UnusedMask = 0x00FFFFFF;
		
		static Vector <IOAPICInfo> * IOAPICs;
		
		static uint32_t ReadRegister ( volatile void * BaseAddress, volatile uint32_t Register );
		static void WriteRegister ( volatile void * BaseAddress, volatile uint32_t Register, uint32_t Value );
		
		
	};
	
};

#endif
