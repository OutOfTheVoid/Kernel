#include <interrupt/PIC.h>
#include <interrupt/IRQ.h>

#include <hw/cpu/IO.h>

void Interrupt::PIC :: Init ()
{
	
	Interrupt::IRQ :: MaxIRQ = 0x0F;
	Interrupt::IRQ :: ExitMode = Interrupt::IRQ :: kIRQExit_PIC;
	
	for ( uint8_t i = 0; i < 0x0F; i ++ )
		SetIRQEnabled ( i, false );
	
};

void Interrupt::PIC :: Remap ( uint32_t LowIRQBase, uint32_t HighIRQBase )
{
	
	uint8_t Addr1 = HW::CPU::IO :: In8 ( kPort_PIC1Data );
	uint8_t Addr2 = HW::CPU::IO :: In8 ( kPort_PIC2Data );
	
	HW::CPU::IO :: Out8 ( kPort_PIC1Command, kPIC_ICW1Init | kPIC_ICW4 );
	HW::CPU::IO :: IOWait ();
	HW::CPU::IO :: Out8 ( kPort_PIC2Command, kPIC_ICW1Init | kPIC_ICW4 );
	HW::CPU::IO :: IOWait ();
	
	HW::CPU::IO :: Out8 ( kPort_PIC1Data, LowIRQBase );
	HW::CPU::IO :: IOWait ();
	HW::CPU::IO :: Out8 ( kPort_PIC2Data, HighIRQBase );
	HW::CPU::IO :: IOWait ();
	
	HW::CPU::IO :: Out8 ( kPort_PIC1Data, 4 );
	HW::CPU::IO :: IOWait ();
	HW::CPU::IO :: Out8 ( kPort_PIC2Data, 2 );
	HW::CPU::IO :: IOWait ();
	
	HW::CPU::IO :: Out8 ( kPort_PIC1Data, kPIC_ICW48086 );
	HW::CPU::IO :: IOWait ();
	HW::CPU::IO :: Out8 ( kPort_PIC2Data, kPIC_ICW48086 );
	HW::CPU::IO :: IOWait ();
	
	HW::CPU::IO :: Out8 ( kPort_PIC1Data, Addr1 );
	HW::CPU::IO :: Out8 ( kPort_PIC2Data, Addr2 );
	
	HW::CPU::IO :: IOWait ();
	
};

void Interrupt::PIC :: Disable ()
{
	
	HW::CPU::IO :: Out8 ( kPort_PIC2Data, kPIC_Disable );
	HW::CPU::IO :: Out8 ( kPort_PIC1Data, kPIC_Disable );
	
	HW::CPU::IO :: IOWait ();
	
};

uint16_t Interrupt::PIC :: GetIRQRequestRegister ()
{
	
	HW::CPU::IO :: Out8 ( kPort_PIC1Command, kPIC_ReadIRR );
	HW::CPU::IO :: Out8 ( kPort_PIC2Command, kPIC_ReadIRR );
	
	return ( static_cast <uint16_t> ( HW::CPU::IO :: In8 ( kPort_PIC2Command ) ) << 8 ) | HW::CPU::IO :: In8 ( kPort_PIC1Command );
	
};

uint16_t Interrupt::PIC :: GetInServiceRegister ()
{
	
	HW::CPU::IO :: Out8 ( kPort_PIC1Command, kPIC_ReadISR );
	HW::CPU::IO :: Out8 ( kPort_PIC2Command, kPIC_ReadISR );
	
	return ( static_cast <uint16_t> ( HW::CPU::IO :: In8 ( kPort_PIC2Command ) ) << 8 ) | HW::CPU::IO :: In8 ( kPort_PIC1Command );
	
};

void Interrupt::PIC :: SetIRQEnabled ( uint8_t IRQLine, bool Enabled )
{
	
	if ( IRQLine > 15 )
		return;
	
	uint8_t Value;
	
	if ( IRQLine < 8 )
	{
		
		Value = HW::CPU::IO :: In8 ( kPort_PIC1Data );
		
		if ( Enabled )
			Value |= ( 1 << IRQLine );
		else
			Value &= ~ ( 1 << IRQLine );
		
		HW::CPU::IO :: Out8 ( kPort_PIC1Data, Value );
		
	}
	else
	{
		
		Value = HW::CPU::IO :: In8 ( kPort_PIC2Data );
		
		IRQLine -= 8;
		
		if ( Enabled )
			Value |= ( 1 << IRQLine );
		else
			Value &= ~ ( 1 << IRQLine );
		
		HW::CPU::IO :: Out8 ( kPort_PIC2Data, Value );
		
	}
	
};

void Interrupt::PIC :: EndOfIRQ ( uint8_t IRQNumber )
{
	
	if ( IRQNumber >= 8 )
		HW::CPU::IO :: Out8 ( kPort_PIC2Data, kPIC_EOI );
	
	HW::CPU::IO :: Out8 ( kPort_PIC1Data, kPIC_EOI );
	
};
