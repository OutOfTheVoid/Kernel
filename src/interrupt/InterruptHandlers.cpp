#include <interrupt/InterruptHandlers.h>
#include <interrupt/IRQ.h>
#include <interrupt/IState.h>

#include <cpputil/linkage.h>

#include <system/func/panic.h>

#include <hw/cpu/IDT.h>
#include <hw/cpu/Processor.h>

#include <mt/hw/TSS.h>
#include <mt/synchronization/Spinlock.h>

#define INTERRUPT_ISR_NAME(number) interrupt_ISR##number
#define INTERRUPT_ISR_EXTERN(number) ASM_LINKAGE void INTERRUPT_ISR_NAME(number) ()

#define INTERRUPT_IRQ_NAME(number) interrupt_IRQ##number
#define INTERRUPT_IRQ_EXTERN(number) ASM_LINKAGE void INTERRUPT_IRQ_NAME(number) ()

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

INTERRUPT_IRQ_EXTERN ( 0x00 );
INTERRUPT_IRQ_EXTERN ( 0x01 );
INTERRUPT_IRQ_EXTERN ( 0x02 );
INTERRUPT_IRQ_EXTERN ( 0x03 );
INTERRUPT_IRQ_EXTERN ( 0x04 );
INTERRUPT_IRQ_EXTERN ( 0x05 );
INTERRUPT_IRQ_EXTERN ( 0x06 );
INTERRUPT_IRQ_EXTERN ( 0x07 );
INTERRUPT_IRQ_EXTERN ( 0x08 );
INTERRUPT_IRQ_EXTERN ( 0x09 );
INTERRUPT_IRQ_EXTERN ( 0x0A );
INTERRUPT_IRQ_EXTERN ( 0x0B );
INTERRUPT_IRQ_EXTERN ( 0x0C );
INTERRUPT_IRQ_EXTERN ( 0x0D );
INTERRUPT_IRQ_EXTERN ( 0x0E );
INTERRUPT_IRQ_EXTERN ( 0x0F );
INTERRUPT_IRQ_EXTERN ( 0x10 );
INTERRUPT_IRQ_EXTERN ( 0x11 );
INTERRUPT_IRQ_EXTERN ( 0x12 );
INTERRUPT_IRQ_EXTERN ( 0x13 );
INTERRUPT_IRQ_EXTERN ( 0x14 );
INTERRUPT_IRQ_EXTERN ( 0x15 );
INTERRUPT_IRQ_EXTERN ( 0x16 );
INTERRUPT_IRQ_EXTERN ( 0x17 );
INTERRUPT_IRQ_EXTERN ( 0x18 );
INTERRUPT_IRQ_EXTERN ( 0x19 );
INTERRUPT_IRQ_EXTERN ( 0x1A );
INTERRUPT_IRQ_EXTERN ( 0x1B );
INTERRUPT_IRQ_EXTERN ( 0x1C );
INTERRUPT_IRQ_EXTERN ( 0x1D );
INTERRUPT_IRQ_EXTERN ( 0x1E );
INTERRUPT_IRQ_EXTERN ( 0x1F );

INTERRUPT_IRQ_EXTERN ( 0xFF ); // For APICs

ASM_LINKAGE void interrupt_InterruptHandlers_CommonISRHandler ( Interrupt::InterruptHandlers :: ISRFrame Frame );

void ( * __Interrupt_HandlerPointers [ 0x40 ] ) ( Interrupt::InterruptHandlers :: ISRFrame * );
MT::Synchronization::Spinlock :: Spinlock_t __Interrupt_HandlerPointerLocks [ 0x40 ];

void Interrupt::InterruptHandlers :: Init ()
{
	
	for ( uint32_t I = 0; I < 0x40; I ++ )
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

	HW::CPU::IDT :: DefineIDTEntry ( Interrupt::IRQ :: kIRQ_BaseInterrupt + 0x00, & INTERRUPT_IRQ_NAME ( 0x00 ), 0x08, HW::CPU::IDT :: kInterruptType_TrapGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( Interrupt::IRQ :: kIRQ_BaseInterrupt + 0x01, & INTERRUPT_IRQ_NAME ( 0x01 ), 0x08, HW::CPU::IDT :: kInterruptType_TrapGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( Interrupt::IRQ :: kIRQ_BaseInterrupt + 0x02, & INTERRUPT_IRQ_NAME ( 0x02 ), 0x08, HW::CPU::IDT :: kInterruptType_TrapGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( Interrupt::IRQ :: kIRQ_BaseInterrupt + 0x03, & INTERRUPT_IRQ_NAME ( 0x03 ), 0x08, HW::CPU::IDT :: kInterruptType_TrapGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( Interrupt::IRQ :: kIRQ_BaseInterrupt + 0x04, & INTERRUPT_IRQ_NAME ( 0x04 ), 0x08, HW::CPU::IDT :: kInterruptType_TrapGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( Interrupt::IRQ :: kIRQ_BaseInterrupt + 0x05, & INTERRUPT_IRQ_NAME ( 0x05 ), 0x08, HW::CPU::IDT :: kInterruptType_TrapGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( Interrupt::IRQ :: kIRQ_BaseInterrupt + 0x06, & INTERRUPT_IRQ_NAME ( 0x06 ), 0x08, HW::CPU::IDT :: kInterruptType_TrapGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( Interrupt::IRQ :: kIRQ_BaseInterrupt + 0x07, & INTERRUPT_IRQ_NAME ( 0x07 ), 0x08, HW::CPU::IDT :: kInterruptType_TrapGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( Interrupt::IRQ :: kIRQ_BaseInterrupt + 0x08, & INTERRUPT_IRQ_NAME ( 0x08 ), 0x08, HW::CPU::IDT :: kInterruptType_TrapGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( Interrupt::IRQ :: kIRQ_BaseInterrupt + 0x09, & INTERRUPT_IRQ_NAME ( 0x09 ), 0x08, HW::CPU::IDT :: kInterruptType_TrapGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( Interrupt::IRQ :: kIRQ_BaseInterrupt + 0x0A, & INTERRUPT_IRQ_NAME ( 0x0A ), 0x08, HW::CPU::IDT :: kInterruptType_TrapGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( Interrupt::IRQ :: kIRQ_BaseInterrupt + 0x0B, & INTERRUPT_IRQ_NAME ( 0x0B ), 0x08, HW::CPU::IDT :: kInterruptType_TrapGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( Interrupt::IRQ :: kIRQ_BaseInterrupt + 0x0C, & INTERRUPT_IRQ_NAME ( 0x0C ), 0x08, HW::CPU::IDT :: kInterruptType_TrapGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( Interrupt::IRQ :: kIRQ_BaseInterrupt + 0x0D, & INTERRUPT_IRQ_NAME ( 0x0D ), 0x08, HW::CPU::IDT :: kInterruptType_TrapGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( Interrupt::IRQ :: kIRQ_BaseInterrupt + 0x0E, & INTERRUPT_IRQ_NAME ( 0x0E ), 0x08, HW::CPU::IDT :: kInterruptType_TrapGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( Interrupt::IRQ :: kIRQ_BaseInterrupt + 0x0F, & INTERRUPT_IRQ_NAME ( 0x0F ), 0x08, HW::CPU::IDT :: kInterruptType_TrapGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( Interrupt::IRQ :: kIRQ_BaseInterrupt + 0x10, & INTERRUPT_IRQ_NAME ( 0x10 ), 0x08, HW::CPU::IDT :: kInterruptType_TrapGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( Interrupt::IRQ :: kIRQ_BaseInterrupt + 0x11, & INTERRUPT_IRQ_NAME ( 0x11 ), 0x08, HW::CPU::IDT :: kInterruptType_TrapGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( Interrupt::IRQ :: kIRQ_BaseInterrupt + 0x12, & INTERRUPT_IRQ_NAME ( 0x12 ), 0x08, HW::CPU::IDT :: kInterruptType_TrapGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( Interrupt::IRQ :: kIRQ_BaseInterrupt + 0x13, & INTERRUPT_IRQ_NAME ( 0x13 ), 0x08, HW::CPU::IDT :: kInterruptType_TrapGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( Interrupt::IRQ :: kIRQ_BaseInterrupt + 0x14, & INTERRUPT_IRQ_NAME ( 0x14 ), 0x08, HW::CPU::IDT :: kInterruptType_TrapGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( Interrupt::IRQ :: kIRQ_BaseInterrupt + 0x15, & INTERRUPT_IRQ_NAME ( 0x15 ), 0x08, HW::CPU::IDT :: kInterruptType_TrapGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( Interrupt::IRQ :: kIRQ_BaseInterrupt + 0x16, & INTERRUPT_IRQ_NAME ( 0x16 ), 0x08, HW::CPU::IDT :: kInterruptType_TrapGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( Interrupt::IRQ :: kIRQ_BaseInterrupt + 0x17, & INTERRUPT_IRQ_NAME ( 0x17 ), 0x08, HW::CPU::IDT :: kInterruptType_TrapGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( Interrupt::IRQ :: kIRQ_BaseInterrupt + 0x18, & INTERRUPT_IRQ_NAME ( 0x18 ), 0x08, HW::CPU::IDT :: kInterruptType_TrapGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( Interrupt::IRQ :: kIRQ_BaseInterrupt + 0x19, & INTERRUPT_IRQ_NAME ( 0x19 ), 0x08, HW::CPU::IDT :: kInterruptType_TrapGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( Interrupt::IRQ :: kIRQ_BaseInterrupt + 0x1A, & INTERRUPT_IRQ_NAME ( 0x1A ), 0x08, HW::CPU::IDT :: kInterruptType_TrapGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( Interrupt::IRQ :: kIRQ_BaseInterrupt + 0x1B, & INTERRUPT_IRQ_NAME ( 0x1B ), 0x08, HW::CPU::IDT :: kInterruptType_TrapGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( Interrupt::IRQ :: kIRQ_BaseInterrupt + 0x1C, & INTERRUPT_IRQ_NAME ( 0x1C ), 0x08, HW::CPU::IDT :: kInterruptType_TrapGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( Interrupt::IRQ :: kIRQ_BaseInterrupt + 0x1D, & INTERRUPT_IRQ_NAME ( 0x1D ), 0x08, HW::CPU::IDT :: kInterruptType_TrapGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( Interrupt::IRQ :: kIRQ_BaseInterrupt + 0x1E, & INTERRUPT_IRQ_NAME ( 0x1E ), 0x08, HW::CPU::IDT :: kInterruptType_TrapGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	HW::CPU::IDT :: DefineIDTEntry ( Interrupt::IRQ :: kIRQ_BaseInterrupt + 0x1F, & INTERRUPT_IRQ_NAME ( 0x1F ), 0x08, HW::CPU::IDT :: kInterruptType_TrapGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	
	HW::CPU::IDT :: DefineIDTEntry ( 0xFF, & INTERRUPT_IRQ_NAME ( 0xFF ), 0x08, HW::CPU::IDT :: kInterruptType_TrapGate32, HW::CPU::IDT :: kPrivelegeLevel_Ring0, true, false );
	
};

void Interrupt::InterruptHandlers :: SetInterruptHandler ( uint32_t InterruptNumber, void ( * Handler ) ( Interrupt::InterruptHandlers :: ISRFrame * ) )
{
	
	if ( InterruptNumber < 0x40 )
	{
		
		bool IBlock = Interrupt::IState :: ReadAndSetBlock ();
		MT::Synchronization::Spinlock :: SpinAcquire ( & __Interrupt_HandlerPointerLocks [ InterruptNumber ] );
		
		__Interrupt_HandlerPointers [ InterruptNumber ] = Handler;
		
		MT::Synchronization::Spinlock :: Release ( & __Interrupt_HandlerPointerLocks [ InterruptNumber ] );
		Interrupt::IState :: WriteBlock ( IBlock );
		
	}
	else
		KPANIC ( "Attempt to set out of range interrupt handler!" );
	
};

void Interrupt::InterruptHandlers :: SetCPInterruptKernelStack ( void * StackTop )
{
	
	HW::CPU::Processor :: CPUInfo * CurrentCPU = HW::CPU::Processor :: GetCurrent ();
	
	bool IBlock = Interrupt::IState :: ReadAndSetBlock ();
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & CurrentCPU -> Lock );
	
	CurrentCPU -> CrossPrivelegeInterruptTSS.ESP0 = reinterpret_cast <uint32_t> ( StackTop );
	CurrentCPU -> CrossPrivelegeInterruptTSS.SS0 = 0x10;
	
	MT::Synchronization::Spinlock :: Release ( & CurrentCPU -> Lock );
	
	Interrupt::IState :: WriteBlock ( IBlock );
	
};

void interrupt_InterruptHandlers_CommonISRHandler ( Interrupt::InterruptHandlers :: ISRFrame Frame )
{
	
	if ( Frame.InterruptNumber < 0x40 )
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
	
	KPANIC ( "Unhandled Interrupt: %i", Frame.InterruptNumber );
	
};
