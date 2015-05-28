#include <hw/cpu/IDT.h>

#include <cpputil/linkage.h>

#include <interrupt/IState.h>

C_LINKAGE void hw_cpu_idtLoad ();

C_LINKAGE HW::CPU::IDT :: IDTR Kernel_IDTR;
C_LINKAGE HW::CPU::IDT :: IDTEntry Kernel_IDTEntries [];

HW::CPU::IDT :: IDTR Kernel_IDTR;
HW::CPU::IDT :: IDTEntry Kernel_IDTEntries [ 0x100 ];

void HW::CPU::IDT :: LoadInterrupts ()
{
	
	Interrupt::IState :: IncrementBlock ();
	
	Kernel_IDTR.Length = 0x100 * sizeof ( IDTEntry ) - 1;
	Kernel_IDTR.Offset = reinterpret_cast <uint32_t> ( & Kernel_IDTEntries );
	
	hw_cpu_idtLoad ();
	
	Interrupt::IState :: DecrementBlock ();
	
};

void HW::CPU::IDT :: DefineIDTEntry ( uint8_t Index, void ( * Offset ) (), uint16_t Selector, InterruptType Type, PrivilegeLevel Privilege, bool Present, bool SegmentPresent )
{
	
	Interrupt::IState :: IncrementBlock ();
	
	Kernel_IDTEntries [ Index ].OffsetLow = static_cast <uint16_t> ( reinterpret_cast <uint32_t> ( Offset ) & 0xFFFF );
	Kernel_IDTEntries [ Index ].OffsetHigh = static_cast <uint16_t> ( reinterpret_cast <uint32_t> ( Offset ) >> 16 );
	
	Kernel_IDTEntries [ Index ].Selector = Selector;
	
	Kernel_IDTEntries [ Index ].Type = static_cast <uint8_t> ( Type ) | static_cast <uint8_t> ( Privilege ) | ( Present ? 0x80 : 0x0 ) | ( SegmentPresent ? 0x10 : 0x00 );
	
	Kernel_IDTEntries [ Index ].Zero = 0;
	
	Interrupt::IState :: DecrementBlock ();
	
};

