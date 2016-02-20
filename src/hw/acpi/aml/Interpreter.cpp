#include <hw/acpi/aml/Interpreter.h>

void ( * HW::ACPI::AML::Interpreter :: OpTable [ 0x100 ] ) ( InterpreterContext * Context );
void ( * HW::ACPI::AML::Interpreter :: ExtOpTable [ 0x100 ] ) ( InterpreterContext * Context );

void HW::ACPI::AML::Interpreter :: Init ()
{
	
	// Type 1 op-codes
	
	OpTable [ 0x33 ] = & BreakPointOp;
	OpTable [ 0x5B ] = & ExtPrefixOp;
	OpTable [ 0x86 ] = & NotifyOp;
	OpTable [ 0x9F ] = & ContinueOp;
	OpTable [ 0xA0 ] = & IfOp;
	OpTable [ 0xA1 ] = & ElseOp;
	OpTable [ 0xA2 ] = & WhileOp;
	OpTable [ 0xA3 ] = & NoOp;
	OpTable [ 0xA4 ] = & ReturnOp;
	OpTable [ 0xA5 ] = & BreakOp;
	
	// Type 2 op-codes
	
	OpTable [ 0x11 ] = & BufferOp;
	OpTable [ 0x72 ] = & AddOp;
	OpTable [ 0x73 ] = & ConcatOp;
	OpTable [ 0x7B ] = & AndOp;
	OpTable [ 0x84 ] = & ConcatResOp;
	OpTable [ 0x ] = & ;
	OpTable [ 0x ] = & ;
	OpTable [ 0x ] = & ;
	OpTable [ 0x ] = & ;
	OpTable [ 0x ] = & ;
	OpTable [ 0x ] = & ;
	OpTable [ 0x ] = & ;
	OpTable [ 0x ] = & ;
	OpTable [ 0x ] = & ;
	OpTable [ 0x ] = & ;
	OpTable [ 0x ] = & ;
	OpTable [ 0x ] = & ;
	OpTable [ 0x ] = & ;
	OpTable [ 0x ] = & ;
	OpTable [ 0x ] = & ;
	OpTable [ 0x ] = & ;
	OpTable [ 0x ] = & ;
	OpTable [ 0x ] = & ;
	
	// Type 1 extended op-codes
	
	ExtOpTable [ 0x20 ] = & LoadOp;
	ExtOpTable [ 0x21 ] = & StallOp;
	ExtOpTable [ 0x22 ] = & SleepOp;
	ExtOpTable [ 0x24 ] = & SignalOp;
	ExtOpTable [ 0x26 ] = & ResetOp;
	ExtOpTable [ 0x27 ] = & ReleaseOp;
	ExtOpTable [ 0x2A ] = & UnloadOp;
	ExtOpTable [ 0x32 ] = & FatalOp;
	
	// Type 2 extended op-codes
	
	ExtOpTable [ 0x12 ] = & CondRefOfOp;
	ExtOpTable [ 0x23 ] = & AcquireOp;
	ExtOpTable [ 0x ] = & ;
	ExtOpTable [ 0x ] = & ;
	ExtOpTable [ 0x ] = & ;
	ExtOpTable [ 0x ] = & ;
	ExtOpTable [ 0x ] = & ;
	ExtOpTable [ 0x ] = & ;
	ExtOpTable [ 0x ] = & ;
	ExtOpTable [ 0x ] = & ;
	ExtOpTable [ 0x ] = & ;
	ExtOpTable [ 0x ] = & ;
	ExtOpTable [ 0x ] = & ;
	ExtOpTable [ 0x ] = & ;
	ExtOpTable [ 0x ] = & ;
	ExtOpTable [ 0x ] = & ;
	ExtOpTable [ 0x ] = & ;
	ExtOpTable [ 0x ] = & ;
	ExtOpTable [ 0x ] = & ;
	ExtOpTable [ 0x ] = & ;
	ExtOpTable [ 0x ] = & ;
	ExtOpTable [ 0x ] = & ;
	ExtOpTable [ 0x ] = & ;
	
};

// Illegal opcode! This should never be called during runtime.

void HW::ACPI::AML::Interpreter :: IllegalOp ( InterpreterContext * Context )
{
	
	
	
};

// ===============
// Type 1 opcodes.
// ===============

void HW::ACPI::AML::Interpreter :: ExtPrefixOp ( InterpreterContext * Context ) // 0x5B
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: BreakOp ( InterpreterContext * Context ) // 0xA5
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: BreakPointOp ( InterpreterContext * Context ) // 0x33
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: ContinueOp ( InterpreterContext * Context ) // 0x9F
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: ElseOp ( InterpreterContext * Context ) // 0xA1
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: IfOp ( InterpreterContext * Context ) // 0xA0
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: NoOp ( InterpreterContext * Context ) // 0xA3
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: NotifyOp ( InterpreterContext * Context ) // 0x86
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: ReturnOp ( InterpreterContext * Context ) // 0xA4
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: WhileOp ( InterpreterContext * Context ) // 0xA2
{
	
	
	
};

// ===============
// Type 1 opcodes.
// ===============

void HW::ACPI::AML::Interpreter :: AddOp ( InterpreterContext * Context ) // 0x72
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: AddOp ( InterpreterContext * Context ) // 0x7B
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: BufferOp ( InterpreterContext * Context ) // 0x11
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: ConcatOp ( InterpreterContext * Context ) // 0x73
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: ConcatResOp ( InterpreterContext * Context ) // 0x84
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: _Op ( InterpreterContext * Context )
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: _Op ( InterpreterContext * Context )
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: _Op ( InterpreterContext * Context )
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: _Op ( InterpreterContext * Context )
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: _Op ( InterpreterContext * Context )
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: _Op ( InterpreterContext * Context )
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: _Op ( InterpreterContext * Context )
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: _Op ( InterpreterContext * Context )
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: _Op ( InterpreterContext * Context )
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: _Op ( InterpreterContext * Context )
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: _Op ( InterpreterContext * Context )
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: _Op ( InterpreterContext * Context )
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: _Op ( InterpreterContext * Context )
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: _Op ( InterpreterContext * Context )
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: _Op ( InterpreterContext * Context )
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: _Op ( InterpreterContext * Context )
{
	
	
	
};

// ===============================
// Extended prefix opcodes, Type 1
// ===============================

void HW::ACPI::AML::Interpreter :: FatalOp ( InterpreterContext * Context ) // Ext 0x32
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: LoadOp ( InterpreterContext * Context ) // Ext 0x20
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: ReleaseOp ( InterpreterContext * Context ) // Ext 0x27
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: ResetOp ( InterpreterContext * Context ) // Ext 0x26
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: SignalOp ( InterpreterContext * Context ) // Ext 0x24
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: SleepOp ( InterpreterContext * Context ) // Ext 0x22
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: StallOp ( InterpreterContext * Context ) // Ext 0x21
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: UnloadOp ( InterpreterContext * Context ) // Ext 0x2A
{
	
	
	
};

// ===============================
// Extended prefix opcodes, Type 2
// ===============================

void HW::ACPI::AML::Interpreter :: AcquireOp ( InterpreterContext * Context ) // Ext 0x23
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: CondRefOfOp ( InterpreterContext * Context ) // Ext 0x12
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: _Op ( InterpreterContext * Context )
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: _Op ( InterpreterContext * Context )
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: _Op ( InterpreterContext * Context )
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: _Op ( InterpreterContext * Context )
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: _Op ( InterpreterContext * Context )
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: _Op ( InterpreterContext * Context )
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: _Op ( InterpreterContext * Context )
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: _Op ( InterpreterContext * Context )
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: _Op ( InterpreterContext * Context )
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: _Op ( InterpreterContext * Context )
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: _Op ( InterpreterContext * Context )
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: _Op ( InterpreterContext * Context )
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: _Op ( InterpreterContext * Context )
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: _Op ( InterpreterContext * Context )
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: _Op ( InterpreterContext * Context )
{
	
	
	
};
