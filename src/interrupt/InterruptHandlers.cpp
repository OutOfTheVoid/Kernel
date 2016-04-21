#include <interrupt/InterruptHandlers.h>
#include <interrupt/IState.h>

#include <cpputil/linkage.h>

#include <system/func/panic.h>

#include <hw/cpu/IDT.h>
#include <hw/cpu/Processor.h>

#include <mt/hw/TSS.h>
#include <mt/synchronization/Spinlock.h>

#define INTERRUPT_ISR_NAME(number) interrupt_ISR##number
#define INTERRUPT_ISR_EXTERN(number) ASM_LINKAGE void INTERRUPT_ISR_NAME(number) ()

INTERRUPT_ISR_EXTERN ( 0x00 );
INTERRUPT_ISR_EXTERN ( 0x01 );
INTERRUPT_ISR_EXTERN ( 0x02 );
INTERRUPT_ISR_EXTERN ( 0x03 );
INTERRUPT_ISR_EXTERN ( 0x04 );
INTERRUPT_ISR_EXTERN ( 0x05 );
INTERRUPT_ISR_EXTERN ( 0x06 );
INTERRUPT_ISR_EXTERN ( 0x07 );
INTERRUPT_ISR_EXTERN ( 0x08 );
INTERRUPT_ISR_EXTERN ( 0x09 );
INTERRUPT_ISR_EXTERN ( 0x0A );
INTERRUPT_ISR_EXTERN ( 0x0B );
INTERRUPT_ISR_EXTERN ( 0x0C );
INTERRUPT_ISR_EXTERN ( 0x0D );
INTERRUPT_ISR_EXTERN ( 0x0E );
INTERRUPT_ISR_EXTERN ( 0x0F );
INTERRUPT_ISR_EXTERN ( 0x10 );
INTERRUPT_ISR_EXTERN ( 0x11 );
INTERRUPT_ISR_EXTERN ( 0x12 );
INTERRUPT_ISR_EXTERN ( 0x13 );
INTERRUPT_ISR_EXTERN ( 0x14 );
INTERRUPT_ISR_EXTERN ( 0x15 );
INTERRUPT_ISR_EXTERN ( 0x16 );
INTERRUPT_ISR_EXTERN ( 0x17 );
INTERRUPT_ISR_EXTERN ( 0x18 );
INTERRUPT_ISR_EXTERN ( 0x19 );
INTERRUPT_ISR_EXTERN ( 0x1A );
INTERRUPT_ISR_EXTERN ( 0x1B );
INTERRUPT_ISR_EXTERN ( 0x1C );
INTERRUPT_ISR_EXTERN ( 0x1D );
INTERRUPT_ISR_EXTERN ( 0x1E );
INTERRUPT_ISR_EXTERN ( 0x1F );
INTERRUPT_ISR_EXTERN ( 0x20 );
INTERRUPT_ISR_EXTERN ( 0x21 );
INTERRUPT_ISR_EXTERN ( 0x22 );
INTERRUPT_ISR_EXTERN ( 0x23 );
INTERRUPT_ISR_EXTERN ( 0x24 );
INTERRUPT_ISR_EXTERN ( 0x25 );
INTERRUPT_ISR_EXTERN ( 0x26 );
INTERRUPT_ISR_EXTERN ( 0x27 );
INTERRUPT_ISR_EXTERN ( 0x28 );
INTERRUPT_ISR_EXTERN ( 0x29 );
INTERRUPT_ISR_EXTERN ( 0x2A );
INTERRUPT_ISR_EXTERN ( 0x2B );
INTERRUPT_ISR_EXTERN ( 0x2C );
INTERRUPT_ISR_EXTERN ( 0x2D );
INTERRUPT_ISR_EXTERN ( 0x2E );
INTERRUPT_ISR_EXTERN ( 0x2F );
INTERRUPT_ISR_EXTERN ( 0x30 );
INTERRUPT_ISR_EXTERN ( 0x31 );
INTERRUPT_ISR_EXTERN ( 0x32 );
INTERRUPT_ISR_EXTERN ( 0x33 );
INTERRUPT_ISR_EXTERN ( 0x34 );
INTERRUPT_ISR_EXTERN ( 0x35 );
INTERRUPT_ISR_EXTERN ( 0x36 );
INTERRUPT_ISR_EXTERN ( 0x37 );
INTERRUPT_ISR_EXTERN ( 0x38 );
INTERRUPT_ISR_EXTERN ( 0x39 );
INTERRUPT_ISR_EXTERN ( 0x3A );
INTERRUPT_ISR_EXTERN ( 0x3B );
INTERRUPT_ISR_EXTERN ( 0x3C );
INTERRUPT_ISR_EXTERN ( 0x3D );
INTERRUPT_ISR_EXTERN ( 0x3E );
INTERRUPT_ISR_EXTERN ( 0x3F );
INTERRUPT_ISR_EXTERN ( 0x40 );
INTERRUPT_ISR_EXTERN ( 0x41 );
INTERRUPT_ISR_EXTERN ( 0x42 );
INTERRUPT_ISR_EXTERN ( 0x43 );
INTERRUPT_ISR_EXTERN ( 0x44 );
INTERRUPT_ISR_EXTERN ( 0x45 );
INTERRUPT_ISR_EXTERN ( 0x46 );
INTERRUPT_ISR_EXTERN ( 0x47 );
INTERRUPT_ISR_EXTERN ( 0x48 );
INTERRUPT_ISR_EXTERN ( 0x49 );
INTERRUPT_ISR_EXTERN ( 0x4A );
INTERRUPT_ISR_EXTERN ( 0x4B );
INTERRUPT_ISR_EXTERN ( 0x4C );
INTERRUPT_ISR_EXTERN ( 0x4D );
INTERRUPT_ISR_EXTERN ( 0x4E );
INTERRUPT_ISR_EXTERN ( 0x4F );
INTERRUPT_ISR_EXTERN ( 0x50 );
INTERRUPT_ISR_EXTERN ( 0x51 );
INTERRUPT_ISR_EXTERN ( 0x52 );
INTERRUPT_ISR_EXTERN ( 0x53 );
INTERRUPT_ISR_EXTERN ( 0x54 );
INTERRUPT_ISR_EXTERN ( 0x55 );
INTERRUPT_ISR_EXTERN ( 0x56 );
INTERRUPT_ISR_EXTERN ( 0x57 );
INTERRUPT_ISR_EXTERN ( 0x58 );
INTERRUPT_ISR_EXTERN ( 0x59 );
INTERRUPT_ISR_EXTERN ( 0x5A );
INTERRUPT_ISR_EXTERN ( 0x5B );
INTERRUPT_ISR_EXTERN ( 0x5C );
INTERRUPT_ISR_EXTERN ( 0x5D );
INTERRUPT_ISR_EXTERN ( 0x5E );
INTERRUPT_ISR_EXTERN ( 0x5F );
INTERRUPT_ISR_EXTERN ( 0x60 );
INTERRUPT_ISR_EXTERN ( 0x61 );
INTERRUPT_ISR_EXTERN ( 0x62 );
INTERRUPT_ISR_EXTERN ( 0x63 );
INTERRUPT_ISR_EXTERN ( 0x64 );
INTERRUPT_ISR_EXTERN ( 0x65 );
INTERRUPT_ISR_EXTERN ( 0x66 );
INTERRUPT_ISR_EXTERN ( 0x67 );
INTERRUPT_ISR_EXTERN ( 0x68 );
INTERRUPT_ISR_EXTERN ( 0x69 );
INTERRUPT_ISR_EXTERN ( 0x6A );
INTERRUPT_ISR_EXTERN ( 0x6B );
INTERRUPT_ISR_EXTERN ( 0x6C );
INTERRUPT_ISR_EXTERN ( 0x6D );
INTERRUPT_ISR_EXTERN ( 0x6E );
INTERRUPT_ISR_EXTERN ( 0x6F );
INTERRUPT_ISR_EXTERN ( 0x70 );
INTERRUPT_ISR_EXTERN ( 0x71 );
INTERRUPT_ISR_EXTERN ( 0x72 );
INTERRUPT_ISR_EXTERN ( 0x73 );
INTERRUPT_ISR_EXTERN ( 0x74 );
INTERRUPT_ISR_EXTERN ( 0x75 );
INTERRUPT_ISR_EXTERN ( 0x76 );
INTERRUPT_ISR_EXTERN ( 0x77 );
INTERRUPT_ISR_EXTERN ( 0x78 );
INTERRUPT_ISR_EXTERN ( 0x79 );
INTERRUPT_ISR_EXTERN ( 0x7A );
INTERRUPT_ISR_EXTERN ( 0x7B );
INTERRUPT_ISR_EXTERN ( 0x7C );
INTERRUPT_ISR_EXTERN ( 0x7D );
INTERRUPT_ISR_EXTERN ( 0x7E );
INTERRUPT_ISR_EXTERN ( 0x7F );
INTERRUPT_ISR_EXTERN ( 0x80 );
INTERRUPT_ISR_EXTERN ( 0x81 );
INTERRUPT_ISR_EXTERN ( 0x82 );
INTERRUPT_ISR_EXTERN ( 0x83 );
INTERRUPT_ISR_EXTERN ( 0x84 );
INTERRUPT_ISR_EXTERN ( 0x85 );
INTERRUPT_ISR_EXTERN ( 0x86 );
INTERRUPT_ISR_EXTERN ( 0x87 );
INTERRUPT_ISR_EXTERN ( 0x88 );
INTERRUPT_ISR_EXTERN ( 0x89 );
INTERRUPT_ISR_EXTERN ( 0x8A );
INTERRUPT_ISR_EXTERN ( 0x8B );
INTERRUPT_ISR_EXTERN ( 0x8C );
INTERRUPT_ISR_EXTERN ( 0x8D );
INTERRUPT_ISR_EXTERN ( 0x8E );
INTERRUPT_ISR_EXTERN ( 0x8F );
INTERRUPT_ISR_EXTERN ( 0x90 );
INTERRUPT_ISR_EXTERN ( 0x91 );
INTERRUPT_ISR_EXTERN ( 0x92 );
INTERRUPT_ISR_EXTERN ( 0x93 );
INTERRUPT_ISR_EXTERN ( 0x94 );
INTERRUPT_ISR_EXTERN ( 0x95 );
INTERRUPT_ISR_EXTERN ( 0x96 );
INTERRUPT_ISR_EXTERN ( 0x97 );
INTERRUPT_ISR_EXTERN ( 0x98 );
INTERRUPT_ISR_EXTERN ( 0x99 );
INTERRUPT_ISR_EXTERN ( 0x9A );
INTERRUPT_ISR_EXTERN ( 0x9B );
INTERRUPT_ISR_EXTERN ( 0x9C );
INTERRUPT_ISR_EXTERN ( 0x9D );
INTERRUPT_ISR_EXTERN ( 0x9E );
INTERRUPT_ISR_EXTERN ( 0x9F );
INTERRUPT_ISR_EXTERN ( 0xA0 );
INTERRUPT_ISR_EXTERN ( 0xA1 );
INTERRUPT_ISR_EXTERN ( 0xA2 );
INTERRUPT_ISR_EXTERN ( 0xA3 );
INTERRUPT_ISR_EXTERN ( 0xA4 );
INTERRUPT_ISR_EXTERN ( 0xA5 );
INTERRUPT_ISR_EXTERN ( 0xA6 );
INTERRUPT_ISR_EXTERN ( 0xA7 );
INTERRUPT_ISR_EXTERN ( 0xA8 );
INTERRUPT_ISR_EXTERN ( 0xA9 );
INTERRUPT_ISR_EXTERN ( 0xAA );
INTERRUPT_ISR_EXTERN ( 0xAB );
INTERRUPT_ISR_EXTERN ( 0xAC );
INTERRUPT_ISR_EXTERN ( 0xAD );
INTERRUPT_ISR_EXTERN ( 0xAE );
INTERRUPT_ISR_EXTERN ( 0xAF );
INTERRUPT_ISR_EXTERN ( 0xB0 );
INTERRUPT_ISR_EXTERN ( 0xB1 );
INTERRUPT_ISR_EXTERN ( 0xB2 );
INTERRUPT_ISR_EXTERN ( 0xB3 );
INTERRUPT_ISR_EXTERN ( 0xB4 );
INTERRUPT_ISR_EXTERN ( 0xB5 );
INTERRUPT_ISR_EXTERN ( 0xB6 );
INTERRUPT_ISR_EXTERN ( 0xB7 );
INTERRUPT_ISR_EXTERN ( 0xB8 );
INTERRUPT_ISR_EXTERN ( 0xB9 );
INTERRUPT_ISR_EXTERN ( 0xBA );
INTERRUPT_ISR_EXTERN ( 0xBB );
INTERRUPT_ISR_EXTERN ( 0xBC );
INTERRUPT_ISR_EXTERN ( 0xBD );
INTERRUPT_ISR_EXTERN ( 0xBE );
INTERRUPT_ISR_EXTERN ( 0xBF );
INTERRUPT_ISR_EXTERN ( 0xC0 );
INTERRUPT_ISR_EXTERN ( 0xC1 );
INTERRUPT_ISR_EXTERN ( 0xC2 );
INTERRUPT_ISR_EXTERN ( 0xC3 );
INTERRUPT_ISR_EXTERN ( 0xC4 );
INTERRUPT_ISR_EXTERN ( 0xC5 );
INTERRUPT_ISR_EXTERN ( 0xC6 );
INTERRUPT_ISR_EXTERN ( 0xC7 );
INTERRUPT_ISR_EXTERN ( 0xC8 );
INTERRUPT_ISR_EXTERN ( 0xC9 );
INTERRUPT_ISR_EXTERN ( 0xCA );
INTERRUPT_ISR_EXTERN ( 0xCB );
INTERRUPT_ISR_EXTERN ( 0xCC );
INTERRUPT_ISR_EXTERN ( 0xCD );
INTERRUPT_ISR_EXTERN ( 0xCE );
INTERRUPT_ISR_EXTERN ( 0xCF );
INTERRUPT_ISR_EXTERN ( 0xD0 );
INTERRUPT_ISR_EXTERN ( 0xD1 );
INTERRUPT_ISR_EXTERN ( 0xD2 );
INTERRUPT_ISR_EXTERN ( 0xD3 );
INTERRUPT_ISR_EXTERN ( 0xD4 );
INTERRUPT_ISR_EXTERN ( 0xD5 );
INTERRUPT_ISR_EXTERN ( 0xD6 );
INTERRUPT_ISR_EXTERN ( 0xD7 );
INTERRUPT_ISR_EXTERN ( 0xD8 );
INTERRUPT_ISR_EXTERN ( 0xD9 );
INTERRUPT_ISR_EXTERN ( 0xDA );
INTERRUPT_ISR_EXTERN ( 0xDB );
INTERRUPT_ISR_EXTERN ( 0xDC );
INTERRUPT_ISR_EXTERN ( 0xDD );
INTERRUPT_ISR_EXTERN ( 0xDE );
INTERRUPT_ISR_EXTERN ( 0xDF );
INTERRUPT_ISR_EXTERN ( 0xE0 );
INTERRUPT_ISR_EXTERN ( 0xE1 );
INTERRUPT_ISR_EXTERN ( 0xE2 );
INTERRUPT_ISR_EXTERN ( 0xE3 );
INTERRUPT_ISR_EXTERN ( 0xE4 );
INTERRUPT_ISR_EXTERN ( 0xE5 );
INTERRUPT_ISR_EXTERN ( 0xE6 );
INTERRUPT_ISR_EXTERN ( 0xE7 );
INTERRUPT_ISR_EXTERN ( 0xE8 );
INTERRUPT_ISR_EXTERN ( 0xE9 );
INTERRUPT_ISR_EXTERN ( 0xEA );
INTERRUPT_ISR_EXTERN ( 0xEB );
INTERRUPT_ISR_EXTERN ( 0xEC );
INTERRUPT_ISR_EXTERN ( 0xED );
INTERRUPT_ISR_EXTERN ( 0xEE );
INTERRUPT_ISR_EXTERN ( 0xEF );
INTERRUPT_ISR_EXTERN ( 0xF0 );
INTERRUPT_ISR_EXTERN ( 0xF1 );
INTERRUPT_ISR_EXTERN ( 0xF2 );
INTERRUPT_ISR_EXTERN ( 0xF3 );
INTERRUPT_ISR_EXTERN ( 0xF4 );
INTERRUPT_ISR_EXTERN ( 0xF5 );
INTERRUPT_ISR_EXTERN ( 0xF6 );
INTERRUPT_ISR_EXTERN ( 0xF7 );
INTERRUPT_ISR_EXTERN ( 0xF8 );
INTERRUPT_ISR_EXTERN ( 0xF9 );
INTERRUPT_ISR_EXTERN ( 0xFA );
INTERRUPT_ISR_EXTERN ( 0xFB );
INTERRUPT_ISR_EXTERN ( 0xFC );
INTERRUPT_ISR_EXTERN ( 0xFD );
INTERRUPT_ISR_EXTERN ( 0xFE );
INTERRUPT_ISR_EXTERN ( 0xFF );

ASM_LINKAGE void interrupt_InterruptHandlers_CommonISRHandler ( Interrupt::InterruptHandlers :: ISRFrame Frame );

void ( * __Interrupt_HandlerPointers [ 0xFF ] ) ( Interrupt::InterruptHandlers :: ISRFrame * );
MT::Synchronization::Spinlock :: Spinlock_t __Interrupt_HandlerPointerLocks [ 0xFF ];

void Interrupt::InterruptHandlers :: Init ()
{
	
	for ( uint32_t I = 0; I < 0xFF; I ++ )
	{
		
		__Interrupt_HandlerPointers [ I ] = NULL;
		__Interrupt_HandlerPointerLocks [ I ] = MT::Synchronization::Spinlock :: Initializer ();
		
	}
	
	InstallSystemInterruptHandlers ();
	
	HW::CPU::IDT :: LoadInterrupts ();
	
};

void Interrupt::InterruptHandlers :: APInit ()
{
	
	HW::CPU::IDT :: LoadInterruptsAP ();
	
};

void Interrupt::InterruptHandlers :: InstallSystemInterruptHandlers ()
{
	
	HW::CPU::IDT :: DefineIDTEntry ( 0x00, & INTERRUPT_ISR_NAME ( 0x00 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x01, & INTERRUPT_ISR_NAME ( 0x01 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x02, & INTERRUPT_ISR_NAME ( 0x02 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x03, & INTERRUPT_ISR_NAME ( 0x03 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x04, & INTERRUPT_ISR_NAME ( 0x04 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x05, & INTERRUPT_ISR_NAME ( 0x05 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x06, & INTERRUPT_ISR_NAME ( 0x06 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x07, & INTERRUPT_ISR_NAME ( 0x07 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x08, & INTERRUPT_ISR_NAME ( 0x08 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x09, & INTERRUPT_ISR_NAME ( 0x09 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x0A, & INTERRUPT_ISR_NAME ( 0x0A ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x0B, & INTERRUPT_ISR_NAME ( 0x0B ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x0C, & INTERRUPT_ISR_NAME ( 0x0C ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x0D, & INTERRUPT_ISR_NAME ( 0x0D ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x0E, & INTERRUPT_ISR_NAME ( 0x0E ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x0F, & INTERRUPT_ISR_NAME ( 0x0F ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x10, & INTERRUPT_ISR_NAME ( 0x10 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x11, & INTERRUPT_ISR_NAME ( 0x11 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x12, & INTERRUPT_ISR_NAME ( 0x12 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x13, & INTERRUPT_ISR_NAME ( 0x13 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x14, & INTERRUPT_ISR_NAME ( 0x14 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x15, & INTERRUPT_ISR_NAME ( 0x15 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x16, & INTERRUPT_ISR_NAME ( 0x16 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x17, & INTERRUPT_ISR_NAME ( 0x17 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x18, & INTERRUPT_ISR_NAME ( 0x18 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x19, & INTERRUPT_ISR_NAME ( 0x19 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x1A, & INTERRUPT_ISR_NAME ( 0x1A ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x1B, & INTERRUPT_ISR_NAME ( 0x1B ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x1C, & INTERRUPT_ISR_NAME ( 0x1C ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x1D, & INTERRUPT_ISR_NAME ( 0x1D ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x1E, & INTERRUPT_ISR_NAME ( 0x1E ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x1F, & INTERRUPT_ISR_NAME ( 0x1F ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x20, & INTERRUPT_ISR_NAME ( 0x20 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x21, & INTERRUPT_ISR_NAME ( 0x21 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x22, & INTERRUPT_ISR_NAME ( 0x22 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x23, & INTERRUPT_ISR_NAME ( 0x23 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x24, & INTERRUPT_ISR_NAME ( 0x24 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x25, & INTERRUPT_ISR_NAME ( 0x25 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x26, & INTERRUPT_ISR_NAME ( 0x26 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x27, & INTERRUPT_ISR_NAME ( 0x27 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x28, & INTERRUPT_ISR_NAME ( 0x28 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x29, & INTERRUPT_ISR_NAME ( 0x29 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x2A, & INTERRUPT_ISR_NAME ( 0x2A ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x2B, & INTERRUPT_ISR_NAME ( 0x2B ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x2C, & INTERRUPT_ISR_NAME ( 0x2C ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x2D, & INTERRUPT_ISR_NAME ( 0x2D ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x2E, & INTERRUPT_ISR_NAME ( 0x2E ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x2F, & INTERRUPT_ISR_NAME ( 0x2F ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x30, & INTERRUPT_ISR_NAME ( 0x30 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x31, & INTERRUPT_ISR_NAME ( 0x31 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x32, & INTERRUPT_ISR_NAME ( 0x32 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x33, & INTERRUPT_ISR_NAME ( 0x33 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x34, & INTERRUPT_ISR_NAME ( 0x34 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x35, & INTERRUPT_ISR_NAME ( 0x35 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x36, & INTERRUPT_ISR_NAME ( 0x36 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x37, & INTERRUPT_ISR_NAME ( 0x37 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x38, & INTERRUPT_ISR_NAME ( 0x38 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x39, & INTERRUPT_ISR_NAME ( 0x39 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x3A, & INTERRUPT_ISR_NAME ( 0x3A ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x3B, & INTERRUPT_ISR_NAME ( 0x3B ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x3C, & INTERRUPT_ISR_NAME ( 0x3C ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x3D, & INTERRUPT_ISR_NAME ( 0x3D ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x3E, & INTERRUPT_ISR_NAME ( 0x3E ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x3F, & INTERRUPT_ISR_NAME ( 0x3F ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x40, & INTERRUPT_ISR_NAME ( 0x40 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x41, & INTERRUPT_ISR_NAME ( 0x41 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x42, & INTERRUPT_ISR_NAME ( 0x42 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x43, & INTERRUPT_ISR_NAME ( 0x43 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x44, & INTERRUPT_ISR_NAME ( 0x44 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x45, & INTERRUPT_ISR_NAME ( 0x45 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x46, & INTERRUPT_ISR_NAME ( 0x46 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x47, & INTERRUPT_ISR_NAME ( 0x47 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x48, & INTERRUPT_ISR_NAME ( 0x48 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x49, & INTERRUPT_ISR_NAME ( 0x49 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x4A, & INTERRUPT_ISR_NAME ( 0x4A ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x4B, & INTERRUPT_ISR_NAME ( 0x4B ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x4C, & INTERRUPT_ISR_NAME ( 0x4C ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x4D, & INTERRUPT_ISR_NAME ( 0x4D ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x4E, & INTERRUPT_ISR_NAME ( 0x4E ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x4F, & INTERRUPT_ISR_NAME ( 0x4F ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x50, & INTERRUPT_ISR_NAME ( 0x50 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x51, & INTERRUPT_ISR_NAME ( 0x51 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x52, & INTERRUPT_ISR_NAME ( 0x52 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x53, & INTERRUPT_ISR_NAME ( 0x53 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x54, & INTERRUPT_ISR_NAME ( 0x54 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x55, & INTERRUPT_ISR_NAME ( 0x55 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x56, & INTERRUPT_ISR_NAME ( 0x56 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x57, & INTERRUPT_ISR_NAME ( 0x57 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x58, & INTERRUPT_ISR_NAME ( 0x58 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x59, & INTERRUPT_ISR_NAME ( 0x59 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x5A, & INTERRUPT_ISR_NAME ( 0x5A ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x5B, & INTERRUPT_ISR_NAME ( 0x5B ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x5C, & INTERRUPT_ISR_NAME ( 0x5C ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x5D, & INTERRUPT_ISR_NAME ( 0x5D ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x5E, & INTERRUPT_ISR_NAME ( 0x5E ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x5F, & INTERRUPT_ISR_NAME ( 0x5F ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x60, & INTERRUPT_ISR_NAME ( 0x60 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x61, & INTERRUPT_ISR_NAME ( 0x61 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x62, & INTERRUPT_ISR_NAME ( 0x62 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x63, & INTERRUPT_ISR_NAME ( 0x63 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x64, & INTERRUPT_ISR_NAME ( 0x64 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x65, & INTERRUPT_ISR_NAME ( 0x65 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x66, & INTERRUPT_ISR_NAME ( 0x66 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x67, & INTERRUPT_ISR_NAME ( 0x67 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x68, & INTERRUPT_ISR_NAME ( 0x68 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x69, & INTERRUPT_ISR_NAME ( 0x69 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x6A, & INTERRUPT_ISR_NAME ( 0x6A ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x6B, & INTERRUPT_ISR_NAME ( 0x6B ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x6C, & INTERRUPT_ISR_NAME ( 0x6C ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x6D, & INTERRUPT_ISR_NAME ( 0x6D ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x6E, & INTERRUPT_ISR_NAME ( 0x6E ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x6F, & INTERRUPT_ISR_NAME ( 0x6F ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x70, & INTERRUPT_ISR_NAME ( 0x70 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x71, & INTERRUPT_ISR_NAME ( 0x71 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x72, & INTERRUPT_ISR_NAME ( 0x72 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x73, & INTERRUPT_ISR_NAME ( 0x73 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x74, & INTERRUPT_ISR_NAME ( 0x74 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x75, & INTERRUPT_ISR_NAME ( 0x75 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x76, & INTERRUPT_ISR_NAME ( 0x76 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x77, & INTERRUPT_ISR_NAME ( 0x77 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x78, & INTERRUPT_ISR_NAME ( 0x78 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x79, & INTERRUPT_ISR_NAME ( 0x79 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x7A, & INTERRUPT_ISR_NAME ( 0x7A ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x7B, & INTERRUPT_ISR_NAME ( 0x7B ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x7C, & INTERRUPT_ISR_NAME ( 0x7C ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x7D, & INTERRUPT_ISR_NAME ( 0x7D ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x7E, & INTERRUPT_ISR_NAME ( 0x7E ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x7F, & INTERRUPT_ISR_NAME ( 0x7F ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	
	HW::CPU::IDT :: DefineIDTEntry ( 0x80, & INTERRUPT_ISR_NAME ( 0x80 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring3, true, false );
	
	HW::CPU::IDT :: DefineIDTEntry ( 0x81, & INTERRUPT_ISR_NAME ( 0x81 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x82, & INTERRUPT_ISR_NAME ( 0x82 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x83, & INTERRUPT_ISR_NAME ( 0x83 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x84, & INTERRUPT_ISR_NAME ( 0x84 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x85, & INTERRUPT_ISR_NAME ( 0x85 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x86, & INTERRUPT_ISR_NAME ( 0x86 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x87, & INTERRUPT_ISR_NAME ( 0x87 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x88, & INTERRUPT_ISR_NAME ( 0x88 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x89, & INTERRUPT_ISR_NAME ( 0x89 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x8A, & INTERRUPT_ISR_NAME ( 0x8A ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x8B, & INTERRUPT_ISR_NAME ( 0x8B ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x8C, & INTERRUPT_ISR_NAME ( 0x8C ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x8D, & INTERRUPT_ISR_NAME ( 0x8D ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x8E, & INTERRUPT_ISR_NAME ( 0x8E ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x8F, & INTERRUPT_ISR_NAME ( 0x8F ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x90, & INTERRUPT_ISR_NAME ( 0x90 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x91, & INTERRUPT_ISR_NAME ( 0x91 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x92, & INTERRUPT_ISR_NAME ( 0x92 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x93, & INTERRUPT_ISR_NAME ( 0x93 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x94, & INTERRUPT_ISR_NAME ( 0x94 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x95, & INTERRUPT_ISR_NAME ( 0x95 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x96, & INTERRUPT_ISR_NAME ( 0x96 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x97, & INTERRUPT_ISR_NAME ( 0x97 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x98, & INTERRUPT_ISR_NAME ( 0x98 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x99, & INTERRUPT_ISR_NAME ( 0x99 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x9A, & INTERRUPT_ISR_NAME ( 0x9A ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x9B, & INTERRUPT_ISR_NAME ( 0x9B ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x9C, & INTERRUPT_ISR_NAME ( 0x9C ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x9D, & INTERRUPT_ISR_NAME ( 0x9D ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x9E, & INTERRUPT_ISR_NAME ( 0x9E ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0x9F, & INTERRUPT_ISR_NAME ( 0x9F ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xA0, & INTERRUPT_ISR_NAME ( 0xA0 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xA1, & INTERRUPT_ISR_NAME ( 0xA1 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xA2, & INTERRUPT_ISR_NAME ( 0xA2 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xA3, & INTERRUPT_ISR_NAME ( 0xA3 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xA4, & INTERRUPT_ISR_NAME ( 0xA4 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xA5, & INTERRUPT_ISR_NAME ( 0xA5 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xA6, & INTERRUPT_ISR_NAME ( 0xA6 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xA7, & INTERRUPT_ISR_NAME ( 0xA7 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xA8, & INTERRUPT_ISR_NAME ( 0xA8 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xA9, & INTERRUPT_ISR_NAME ( 0xA9 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xAA, & INTERRUPT_ISR_NAME ( 0xAA ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xAB, & INTERRUPT_ISR_NAME ( 0xAB ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xAC, & INTERRUPT_ISR_NAME ( 0xAC ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xAD, & INTERRUPT_ISR_NAME ( 0xAD ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xAE, & INTERRUPT_ISR_NAME ( 0xAE ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xAF, & INTERRUPT_ISR_NAME ( 0xAF ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xB0, & INTERRUPT_ISR_NAME ( 0xB0 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xB1, & INTERRUPT_ISR_NAME ( 0xB1 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xB2, & INTERRUPT_ISR_NAME ( 0xB2 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xB3, & INTERRUPT_ISR_NAME ( 0xB3 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xB4, & INTERRUPT_ISR_NAME ( 0xB4 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xB5, & INTERRUPT_ISR_NAME ( 0xB5 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xB6, & INTERRUPT_ISR_NAME ( 0xB6 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xB7, & INTERRUPT_ISR_NAME ( 0xB7 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xB8, & INTERRUPT_ISR_NAME ( 0xB8 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xB9, & INTERRUPT_ISR_NAME ( 0xB9 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xBA, & INTERRUPT_ISR_NAME ( 0xBA ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xBB, & INTERRUPT_ISR_NAME ( 0xBB ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xBC, & INTERRUPT_ISR_NAME ( 0xBC ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xBD, & INTERRUPT_ISR_NAME ( 0xBD ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xBE, & INTERRUPT_ISR_NAME ( 0xBE ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xBF, & INTERRUPT_ISR_NAME ( 0xBF ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xC0, & INTERRUPT_ISR_NAME ( 0xC0 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xC1, & INTERRUPT_ISR_NAME ( 0xC1 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xC2, & INTERRUPT_ISR_NAME ( 0xC2 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xC3, & INTERRUPT_ISR_NAME ( 0xC3 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xC4, & INTERRUPT_ISR_NAME ( 0xC4 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xC5, & INTERRUPT_ISR_NAME ( 0xC5 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xC6, & INTERRUPT_ISR_NAME ( 0xC6 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xC7, & INTERRUPT_ISR_NAME ( 0xC7 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xC8, & INTERRUPT_ISR_NAME ( 0xC8 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xC9, & INTERRUPT_ISR_NAME ( 0xC9 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xCA, & INTERRUPT_ISR_NAME ( 0xCA ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xCB, & INTERRUPT_ISR_NAME ( 0xCB ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xCC, & INTERRUPT_ISR_NAME ( 0xCC ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xCD, & INTERRUPT_ISR_NAME ( 0xCD ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xCE, & INTERRUPT_ISR_NAME ( 0xCE ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xCF, & INTERRUPT_ISR_NAME ( 0xCF ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xD0, & INTERRUPT_ISR_NAME ( 0xD0 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xD1, & INTERRUPT_ISR_NAME ( 0xD1 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xD2, & INTERRUPT_ISR_NAME ( 0xD2 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xD3, & INTERRUPT_ISR_NAME ( 0xD3 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xD4, & INTERRUPT_ISR_NAME ( 0xD4 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xD5, & INTERRUPT_ISR_NAME ( 0xD5 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xD6, & INTERRUPT_ISR_NAME ( 0xD6 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xD7, & INTERRUPT_ISR_NAME ( 0xD7 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xD8, & INTERRUPT_ISR_NAME ( 0xD8 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xD9, & INTERRUPT_ISR_NAME ( 0xD9 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xDA, & INTERRUPT_ISR_NAME ( 0xDA ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xDB, & INTERRUPT_ISR_NAME ( 0xDB ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xDC, & INTERRUPT_ISR_NAME ( 0xDC ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xDD, & INTERRUPT_ISR_NAME ( 0xDD ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xDE, & INTERRUPT_ISR_NAME ( 0xDE ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xDF, & INTERRUPT_ISR_NAME ( 0xDF ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xE0, & INTERRUPT_ISR_NAME ( 0xE0 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xE1, & INTERRUPT_ISR_NAME ( 0xE1 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xE2, & INTERRUPT_ISR_NAME ( 0xE2 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xE3, & INTERRUPT_ISR_NAME ( 0xE3 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xE4, & INTERRUPT_ISR_NAME ( 0xE4 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xE5, & INTERRUPT_ISR_NAME ( 0xE5 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xE6, & INTERRUPT_ISR_NAME ( 0xE6 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xE7, & INTERRUPT_ISR_NAME ( 0xE7 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xE8, & INTERRUPT_ISR_NAME ( 0xE8 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xE9, & INTERRUPT_ISR_NAME ( 0xE9 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xEA, & INTERRUPT_ISR_NAME ( 0xEA ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xEB, & INTERRUPT_ISR_NAME ( 0xEB ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xEC, & INTERRUPT_ISR_NAME ( 0xEC ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xED, & INTERRUPT_ISR_NAME ( 0xED ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xEE, & INTERRUPT_ISR_NAME ( 0xEE ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xEF, & INTERRUPT_ISR_NAME ( 0xEF ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xF0, & INTERRUPT_ISR_NAME ( 0xF0 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xF1, & INTERRUPT_ISR_NAME ( 0xF1 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xF2, & INTERRUPT_ISR_NAME ( 0xF2 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xF3, & INTERRUPT_ISR_NAME ( 0xF3 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xF4, & INTERRUPT_ISR_NAME ( 0xF4 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xF5, & INTERRUPT_ISR_NAME ( 0xF5 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xF6, & INTERRUPT_ISR_NAME ( 0xF6 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xF7, & INTERRUPT_ISR_NAME ( 0xF7 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xF8, & INTERRUPT_ISR_NAME ( 0xF8 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xF9, & INTERRUPT_ISR_NAME ( 0xF9 ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xFA, & INTERRUPT_ISR_NAME ( 0xFA ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xFB, & INTERRUPT_ISR_NAME ( 0xFB ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xFC, & INTERRUPT_ISR_NAME ( 0xFC ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xFD, & INTERRUPT_ISR_NAME ( 0xFD ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xFE, & INTERRUPT_ISR_NAME ( 0xFE ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( 0xFF, & INTERRUPT_ISR_NAME ( 0xFF ), 0x08, HW::CPU::IDT :: kInterruptType_InterruptGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	
};

void Interrupt::InterruptHandlers :: SetInterruptHandler ( uint32_t InterruptNumber, void ( * Handler ) ( Interrupt::InterruptHandlers :: ISRFrame * ) )
{
	
	if ( InterruptNumber < 0x100 )
	{
		
		MT::Synchronization::Spinlock :: SpinAcquire ( & __Interrupt_HandlerPointerLocks [ InterruptNumber ] );
		
		__Interrupt_HandlerPointers [ InterruptNumber ] = Handler;
		
		MT::Synchronization::Spinlock :: Release ( & __Interrupt_HandlerPointerLocks [ InterruptNumber ] );
		
	}
	else
		KPANIC ( "Attempt to set out of range interrupt handler!" );
	
};

void interrupt_InterruptHandlers_CommonISRHandler ( Interrupt::InterruptHandlers :: ISRFrame Frame )
{
	
	if ( Frame.InterruptNumber < 0x100 )
	{
		
		MT::Synchronization::Spinlock :: SpinAcquire ( & __Interrupt_HandlerPointerLocks [ Frame.InterruptNumber ] );
		
		if ( __Interrupt_HandlerPointers [ Frame.InterruptNumber ] != NULL )
		{
			
			void ( * Handler ) ( Interrupt::InterruptHandlers :: ISRFrame * Frame ) = __Interrupt_HandlerPointers [ Frame.InterruptNumber ];
			
			MT::Synchronization::Spinlock :: Release ( & __Interrupt_HandlerPointerLocks [ Frame.InterruptNumber ] );
			
			return Handler ( & Frame );
			
		}
		
		MT::Synchronization::Spinlock :: Release ( & __Interrupt_HandlerPointerLocks [ Frame.InterruptNumber ] );
		
	}
	
	KPANIC ( "Unhandled Interrupt! [ %h ]\n", Frame.InterruptNumber );
	
};
