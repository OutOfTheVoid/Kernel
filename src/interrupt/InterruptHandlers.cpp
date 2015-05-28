#include <interrupt/InterruptHandlers.h>
#include <interrupt/IRQ.h>

#include <cpputil/linkage.h>

#include <system/func/panic.h>

#include <hw/cpu/IDT.h>

#define INTERRUPT_ISR_NAME(number) interrupt_ISR##number
#define INTERRUPT_ISR_EXTERN(number) C_LINKAGE void INTERRUPT_ISR_NAME(number) ()

#define INTERRUPT_IRQ_NAME(number) interrupt_IRQ##number
#define INTERRUPT_IRQ_EXTERN(number) C_LINKAGE void INTERRUPT_IRQ_NAME(number) ()

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

C_LINKAGE void interrupt_InterruptHandlers_CommonISRHandler ( Interrupt::InterruptHandlers :: ISRFrame );

void Interrupt::InterruptHandlers :: Init ()
{
	
	InstallSystemInterruptHandlers ();
	
	HW::CPU::IDT :: LoadInterrupts ();
	
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
	
};

void interrupt_InterruptHandlers_CommonISRHandler ( Interrupt::InterruptHandlers :: ISRFrame Frame )
{
};
