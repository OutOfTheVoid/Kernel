#ifndef HW_CPU_IDT_H
#define HW_CPU_IDT_H

#include <hw/HW.h>
#include <hw/cpu/CPU.h>

#include <stdint.h>

namespace HW
{
	
	namespace CPU
	{
		
		class IDT
		{
		public:
			
			typedef struct
			{
				
				uint16_t Length;
				uint32_t Offset;
				
			} __attribute__((packed)) IDTR;
			
			typedef struct
			{
				
				uint16_t OffsetLow;
				uint16_t Selector;
				
				uint8_t Zero;
				uint8_t Type;
				
				uint16_t OffsetHigh;
				
			} __attribute__((packed)) IDTEntry;
			
			typedef enum
			{
				
				kInterruptType_TaskGage32 = 0x05,
				kInterruptType_InterruptGate16 = 0x06,
				kInterruptType_TrapGate16 = 0x07,
				kInterruptType_InterruptGate32 = 0x0E,
				kInterruptType_TrapGate32 = 0x0F
				
			} InterruptType;
			
			typedef enum
			{
				
				kPrivelegeLevel_Ring0 = 0x00,
				kPrivelegeLevel_Ring1 = 0x20,
				kPrivelegeLevel_Ring2 = 0x40,
				kPrivelegeLevel_Ring3 = 0x60
				
			} PrivilegeLevel;
			
			static void LoadInterrupts ();
			static void LoadInterruptsAP ();
			
			static void DefineIDTEntry ( uint8_t Index, void ( * Offset ) (), uint16_t Selector, InterruptType Type, PrivilegeLevel Privelege, bool Present, bool SegmentPresent );
			
		private:
			
		};
		
	};
	
};

#endif
