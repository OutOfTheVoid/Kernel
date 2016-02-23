#include <hw/acpi/aml/Interpreter.h>

void ( * HW::ACPI::AML::Interpreter :: OpTable [ 0x100 ] ) ( InterpreterContext * Context );
void ( * HW::ACPI::AML::Interpreter :: ExtOpTable [ 0x100 ] ) ( InterpreterContext * Context );

void HW::ACPI::AML::Interpreter :: Init ()
{
	
	for ( uint32_t I = 0; I < 0x100; I ++ )
	{
		
		OpTable [ I ] = & IllegalOp;
		ExtOpTable [ I ] = & IllegalOp;
		
	}
	
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
	
	// Type 1 extended op-codes
	
	ExtOpTable [ 0x20 ] = & LoadOp;
	ExtOpTable [ 0x21 ] = & StallOp;
	ExtOpTable [ 0x22 ] = & SleepOp;
	ExtOpTable [ 0x24 ] = & SignalOp;
	ExtOpTable [ 0x26 ] = & ResetOp;
	ExtOpTable [ 0x27 ] = & ReleaseOp;
	ExtOpTable [ 0x2A ] = & UnloadOp;
	ExtOpTable [ 0x32 ] = & FatalOp;
	
	// Type 2 op-codes
	
	OpTable [ 0x11 ] = & BufferOp;
	OpTable [ 0x12 ] = & PackageOp;
	OpTable [ 0x13 ] = & VarPackageOp;
	OpTable [ 0x70 ] = & StoreOp;
	OpTable [ 0x71 ] = & RefOfOp;
	OpTable [ 0x72 ] = & AddOp;
	OpTable [ 0x73 ] = & ConcatOp;
	OpTable [ 0x74 ] = & SubtractOp;
	OpTable [ 0x75 ] = & IncrementOp;
	OpTable [ 0x76 ] = & DecrementOp;
	OpTable [ 0x77 ] = & MultiplyOp;
	OpTable [ 0x78 ] = & DivideOp;
	OpTable [ 0x79 ] = & ShiftLeftOp;
	OpTable [ 0x7A ] = & ShiftRightOp; // ----
	OpTable [ 0x7B ] = & AndOp;
	OpTable [ 0x7C ] = & NandOp;
	OpTable [ 0x7D ] = & OrOp;
	OpTable [ 0x7E ] = & NorOp;
	OpTable [ 0x75 ] = & XorOp;
	OpTable [ 0x80 ] = & NotOp;
	OpTable [ 0x81 ] = & FindSetLeftBitOp;
	OpTable [ 0x82 ] = & FindSetRightBitOp;
	OpTable [ 0x83 ] = & DerefOfOp;
	OpTable [ 0x84 ] = & ConcatResOp;
	OpTable [ 0x85 ] = & ModOp;
	OpTable [ 0x87 ] = & SizeOfOp;
	OpTable [ 0x88 ] = & IndexOp;
	OpTable [ 0x89 ] = & MatchOp;
	OpTable [ 0x8E ] = & ObjectTypeOp;
	OpTable [ 0x90 ] = & LAndOp;
	OpTable [ 0x91 ] = & LOrOp;
	OpTable [ 0x92 ] = & LNotOp;
	OpTable [ 0x93 ] = & LEqualOp;
	OpTable [ 0x94 ] = & LGreaterOp;
	OpTable [ 0x95 ] = & LLessOp;
	OpTable [ 0x96 ] = & ToBufferOp;
	OpTable [ 0x97 ] = & ToDecimalStringOp;
	OpTable [ 0x98 ] = & ToHexStringOp;
	OpTable [ 0x99 ] = & ToIntegerOp;
	OpTable [ 0x9C ] = & ToStringOp;
	OpTable [ 0x9D ] = & CopyObjectOp;
	OpTable [ 0x9E ] = & MidOp;
	
	// Type 2 extended op-codes
	
	ExtOpTable [ 0x12 ] = & CondRefOfOp;
	ExtOpTable [ 0x23 ] = & AcquireOp;
	ExtOpTable [ 0x28 ] = & FromBCDOp;
	ExtOpTable [ 0x1F ] = & LoadTableOp;
	ExtOpTable [ 0x33 ] = & TimerOp;
	ExtOpTable [ 0x29 ] = & ToBCDOp;
	ExtOpTable [ 0x25 ] = & WaitOp;
	
};

void HW::ACPI::AML::Interpreter :: Exec ( InterpreterContext * Context, uint32_t * Status )
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: EvaluateTermArg ( InterpreterContext * Context, Object :: ACPIObject * Result )
{
	
	MethodInvocationContext * MethodContext = & Context -> MethodContextStack [ Context -> MethodContextStackIndex ];
	
	if ( MethodContext -> CurrentOffset + 2 > MethodContext -> MaxOffset )
	{
		
		Context -> State = kInterpreterState_Error_Bounds;
		Result -> Type = Object :: kObjectType_Uninitialized;
		
		return;
		
	}
	
	uint8_t PrefixByte = reinterpret_cast <uint8_t *> ( Context -> Block.Data ) [ MethodContext -> CurrentOffset ];
	
	MethodContext -> CurrentOffset ++;
	
	union
	{
		
		uint8_t ByteData;
		uint16_t WordData;
		uint32_t DWordData;
		uint64_t QWordData;
		
		struct
		{
			
			uint32_t StringLength;
			uint32_t StringOffset;
			
		} StringFigures;
		
	};
	
	switch ( PrefixByte )
	{
	
	case 0x0A: // Byte prefix
		
		ByteData = reinterpret_cast <uint8_t *> ( Context -> Block.Data ) [ MethodContext -> CurrentOffset ];
		
		Result -> Type = MethodContext -> IntegerSizeIs64 ? Object :: kObjectType_Int64 : Object :: kObjectType_Int32;
		
		MethodContext -> IntegerSizeIs64 ? Result -> Value.Int64 = static_cast <uint64_t> ( ByteData ) : Result -> Value.Int32 = static_cast <uint32_t> ( ByteData );
		
		break;
		
	case 0x0B: // Word prefix
	
		if ( MethodContext -> CurrentOffset + 2 > MethodContext -> MaxOffset )
		{
			
			Context -> State = kInterpreterState_Error_Bounds;
			Result -> Type = Object :: kObjectType_Uninitialized;
			
			return;
			
		}
		
		WordData = * reinterpret_cast <uint16_t *> ( reinterpret_cast <char *> ( Context -> Block.Data ) + reinterpret_cast <size_t> ( MethodContext -> CurrentOffset ) );
		
		Result -> Type = MethodContext -> IntegerSizeIs64 ? Object :: kObjectType_Int64 : Object :: kObjectType_Int32;
		
		MethodContext -> IntegerSizeIs64 ? Result -> Value.Int64 = static_cast <uint64_t> ( WordData ) : Result -> Value.Int32 = static_cast <uint32_t> ( WordData );
		
		break;
		
	case 0x0C: // DWord prefix
		
		if ( MethodContext -> CurrentOffset + 4 > MethodContext -> MaxOffset )
		{
			
			Context -> State = kInterpreterState_Error_Bounds;
			Result -> Type = Object :: kObjectType_Uninitialized;
			
			return;
			
		}
		
		DWordData = * reinterpret_cast <uint32_t *> ( reinterpret_cast <char *> ( Context -> Block.Data ) + reinterpret_cast <size_t> ( MethodContext -> CurrentOffset ) );
		
		Result -> Type = MethodContext -> IntegerSizeIs64 ? Object :: kObjectType_Int64 : Object :: kObjectType_Int32;
		
		MethodContext -> IntegerSizeIs64 ? Result -> Value.Int64 = static_cast <uint64_t> ( DWordData ) : Result -> Value.Int32 = static_cast <uint32_t> ( DWordData );
		
		break;
		
	case 0x0E: // QWord prefix
		
		if ( MethodContext -> CurrentOffset + 8 > MethodContext -> MaxOffset )
		{
			
			Context -> State = kInterpreterState_Error_Bounds;
			Result -> Type = Object :: kObjectType_Uninitialized;
			
			return;
			
		}
		
		QWordData = * reinterpret_cast <uint64_t *> ( reinterpret_cast <char *> ( Context -> Block.Data ) + reinterpret_cast <size_t> ( MethodContext -> CurrentOffset ) );
		
		Result -> Type = MethodContext -> IntegerSizeIs64 ? Object :: kObjectType_Int64 : Object :: kObjectType_Int32;
		
		MethodContext -> IntegerSizeIs64 ? Result -> Value.Int64 = static_cast <uint64_t> ( QWordData ) : Result -> Value.Int32 = static_cast <uint32_t> ( QWordData );
		
		break;
		
	case 0x0D:
		
		
		StringFigures.StringLength = 0;
		StringFigures.StringOffset = MethodContext -> CurrentOffset;
		
		while ( ( MethodContext -> CurrentOffset + 1 > MethodContext -> MaxOffset ) && ( * reinterpret_cast <uint8_t *> ( reinterpret_cast <char *> ( Context -> Block.Data ) + reinterpret_cast <size_t> ( MethodContext -> CurrentOffset ) ) != 0 ) )
		{
			
			if ( * reinterpret_cast <uint8_t *> ( reinterpret_cast <char *> ( Context -> Block.Data ) + reinterpret_cast <size_t> ( MethodContext -> CurrentOffset ) ) > 0x7F )
			{
				
				Context -> State = kInterpreterState_Error_MalformedConstant;
				Result -> Type = Object :: kObjectType_Uninitialized;
				
				return;
				
			}
			
			StringFigures.StringLength ++;
			MethodContext -> CurrentOffset ++;
			
		}
		
		if ( MethodContext -> CurrentOffset + 1 <= MethodContext -> MaxOffset )
		{
			
			Context -> State = kInterpreterState_Error_Bounds;
			Result -> Type = Object :: kObjectType_Uninitialized;
			
			return;
			
		}
		
		Result -> Type = Object :: kObjectType_String;
		
		Result -> Value.String.CString = reinterpret_cast <const char *> ( reinterpret_cast <char *> ( Context -> Block.Data ) + reinterpret_cast <size_t> ( StringFigures.StringOffset ) );
		Result -> Value.String.Length = StringFigures.StringLength;
		Result -> Value.String.Allocated = false;
		
		break;
		
	}
	
};

// Illegal opcode! This should never be called during runtime.

void HW::ACPI::AML::Interpreter :: IllegalOp ( InterpreterContext * Context )
{
	
	Context -> State = kInterpreterState_Error_IllegalOpcode;
	
};

// ===============
// Type 1 opcodes.
// ===============

void HW::ACPI::AML::Interpreter :: ExtPrefixOp ( InterpreterContext * Context ) // 0x5B
{
	
	MethodInvocationContext * MethodContext = & Context -> MethodContextStack [ Context -> MethodContextStackIndex ];
	MethodContext -> CurrentOffset ++;
	
	if ( MethodContext -> CurrentOffset > MethodContext -> MaxOffset )
	{
		
		Context -> State = kInterpreterState_Error_Bounds;
		
		return;
		
	}
	
	uint8_t ExtensionOpcode = reinterpret_cast <uint8_t *> ( Context -> Block.Data ) [ MethodContext -> CurrentOffset ];
	
	return ExtOpTable [ ExtensionOpcode ] ( Context );
	
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
// Type 2 opcodes.
// ===============

void HW::ACPI::AML::Interpreter :: AddOp ( InterpreterContext * Context ) // 0x72
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: AndOp ( InterpreterContext * Context ) // 0x7B
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

void HW::ACPI::AML::Interpreter :: CopyObjectOp ( InterpreterContext * Context ) // 0x9D
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: DecrementOp ( InterpreterContext * Context ) // 0x76
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: DerefOfOp ( InterpreterContext * Context ) // 0x83
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: DivideOp ( InterpreterContext * Context ) // 0x78
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: FindSetLeftBitOp ( InterpreterContext * Context ) // 0x81
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: FindSetRightBitOp ( InterpreterContext * Context ) // 0x82
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: IncrementOp ( InterpreterContext * Context ) // 0x75
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: IndexOp ( InterpreterContext * Context ) // 0x88
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: LAndOp ( InterpreterContext * Context ) // 0x90
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: LEqualOp ( InterpreterContext * Context ) // 0x93
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: LGreaterOp ( InterpreterContext * Context ) // 0x94
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: LLessOp ( InterpreterContext * Context ) // 0x95
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: LNotOp ( InterpreterContext * Context ) // 0x92
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: LOrOp ( InterpreterContext * Context ) // 0x91
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: MatchOp ( InterpreterContext * Context ) // 0x89
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: MidOp ( InterpreterContext * Context ) // 0x9E
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: ModOp ( InterpreterContext * Context ) // 0x85
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: MultiplyOp ( InterpreterContext * Context ) // 0x77
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: NandOp ( InterpreterContext * Context ) // 0x7C
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: NorOp ( InterpreterContext * Context ) // 0x7E
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: NotOp ( InterpreterContext * Context ) // 0x80
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: ObjectTypeOp ( InterpreterContext * Context ) // 0x8E
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: OrOp ( InterpreterContext * Context ) // 0x7D
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: PackageOp ( InterpreterContext * Context ) // 0x12
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: VarPackageOp ( InterpreterContext * Context ) // 0x13
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: RefOfOp ( InterpreterContext * Context ) // 0x71
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: ShiftLeftOp ( InterpreterContext * Context ) // 0x79
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: ShiftRightOp ( InterpreterContext * Context ) // 0x7A
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: SizeOfOp ( InterpreterContext * Context ) // 0x87
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: StoreOp ( InterpreterContext * Context ) // 0x70
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: SubtractOp ( InterpreterContext * Context ) // 0x74
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: ToBufferOp ( InterpreterContext * Context ) // 0x96
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: ToDecimalStringOp ( InterpreterContext * Context ) // 0x97
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: ToHexStringOp ( InterpreterContext * Context ) // 0x98
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: ToIntegerOp ( InterpreterContext * Context ) // 0x99
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: ToStringOp ( InterpreterContext * Context ) // 0x9C
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: XorOp ( InterpreterContext * Context ) // 0x7F
{
	
	
	
};

// ===============================
// Extended prefix opcodes, Type 1
// ===============================

void HW::ACPI::AML::Interpreter :: FatalOp ( InterpreterContext * Context ) // Ext 0x32
{
	
	MethodInvocationContext * MethodContext = & Context -> MethodContextStack [ Context -> MethodContextStackIndex ];
	MethodContext -> CurrentOffset ++;
	
	if ( MethodContext -> CurrentOffset + 1 > MethodContext -> MaxOffset )
	{
		
		Context -> State = kInterpreterState_Error_Bounds;
		
		return;
		
	}
	
	uint8_t Type;
	uint32_t Code;
	Object :: ACPIObject Argument;
	
	Type = * reinterpret_cast <uint8_t *> ( reinterpret_cast <char *> ( Context -> Block.Data ) + reinterpret_cast <size_t> ( MethodContext -> CurrentOffset ) );
	MethodContext -> CurrentOffset ++;
	
	if ( MethodContext -> CurrentOffset + 4 > MethodContext -> MaxOffset )
	{
		
		Context -> State = kInterpreterState_Error_Bounds;
		
		return;
		
	}
	
	Code = * reinterpret_cast <uint32_t *> ( reinterpret_cast <char *> ( Context -> Block.Data ) + reinterpret_cast <size_t> ( MethodContext -> CurrentOffset ) );
	MethodContext -> CurrentOffset += 4;
	
	uint32_t PreState = Context -> State;
	
	EvaluateTermArg ( Context, & Argument );
	
	if ( Context -> State != PreState )
		return;
	
	Context -> FatalError.Type = Type;
	Context -> FatalError.Code = Code;
	
	
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

void HW::ACPI::AML::Interpreter :: FromBCDOp ( InterpreterContext * Context ) // Ext 0x28
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: LoadTableOp ( InterpreterContext * Context ) // Ext 0x1F
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: TimerOp ( InterpreterContext * Context ) // Ext 0x33
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: ToBCDOp ( InterpreterContext * Context ) // Ext 0x29
{
	
	
	
};

void HW::ACPI::AML::Interpreter :: WaitOp ( InterpreterContext * Context ) // Ext 0x25
{
	
	
	
};
