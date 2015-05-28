#ifndef INTERRUPT_PIC_H
#define INTERRUPT_PIC_H

#include <interrupt/Interrupt.h>

#include <stdint.h>

namespace Interrupt
{
	
	class PIC
	{
	public:
		
		static void Init ();
		
		static void Remap ( uint32_t LowIRQBase, uint32_t HighIRQBase );	
		static void SetIRQEnabled ( uint8_t IRQLine, bool Enabled );
		
		static void Disable ();
		
		static void EndOfIRQ ( uint8_t IRQNumber );
		
		static uint16_t GetIRQRequestRegister ();
		static uint16_t GetInServiceRegister ();
	
	private:
	
		static const uint16_t kPort_PIC1Base = 0x0020;
		static const uint16_t kPort_PIC1Command = kPort_PIC1Base;
		static const uint16_t kPort_PIC1Data = kPort_PIC1Base + 1;
		
		static const uint16_t kPort_PIC2Base = 0x00A0;
		static const uint16_t kPort_PIC2Command = kPort_PIC2Base;
		static const uint16_t kPort_PIC2Data = kPort_PIC2Base + 1;
		
		static const uint8_t kPIC_ICW4 = 0x01;
		static const uint8_t kPIC_ICW1Init = 0x10;
		static const uint8_t kPIC_ICW48086 = 0x01;
		static const uint8_t kPIC_EOI = 0x20;
		static const uint8_t kPIC_ReadIRR = 0x0A;
		static const uint8_t kPIC_ReadISR = 0x0B;
		static const uint8_t kPIC_Disable = 0xFF;
		
	};
	
};

#endif
