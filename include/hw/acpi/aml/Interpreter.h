#ifndef HW_ACPI_AML_INTERPRETER_H
#define HW_ACPI_AML_INTERPRETER_H

#include <hw/acpi/aml/Object.h>

#include <hw/HW.h>
#include <hw/acpi/ACPI.h>
#include <hw/acpi/aml/AML.h>

#include <stdint.h>

namespace HW
{
	
	namespace ACPI
	{
		
		namespace AML
		{
			
			class Interpreter
			{
			public:
				
				static const uint32_t kInterpreterStatus_Success = 0;
				
				static const uint32_t kInterpreterStatus_Create_Failure_OutOfMemory = 1;
				
				typedef void * InterpreterHandle;
				
				static void Init ();
				
				static InterpreterHandle CreateInterpreter ( uint32_t * Status );
				
				static void RunControlMethod (  );
				
			private:
				
				static const uint32_t kMaxInvocationContextStack = 0x20;
				static const uint32_t kMaxFlowControlModeStack = 0x200;
				
				// Execution statuses
				
				static const uint32_t kExecutionStatus_Finished = 0;
				
				static const uint32_t kExecutionStatus_Blocked = 1;
				
				static const uint32_t kExecutionStatus_Failure = 2;
				
				// Interpreter states
				
				static const uint32_t kInterpreterState_Idle = 0;
				
				static const uint32_t kInterpreterState_Executing = 1;
				
				static const uint32_t kInterpreterState_Error_Thrown = 2;
				
				static const uint32_t kInterpreterState_Error_Unknown = 3;
				static const uint32_t kInterpreterState_Error_Bounds = 4;
				static const uint32_t kInterpreterState_Error_IllegalOpcode = 5;
				static const uint32_t kInterpreterState_Error_MalformedConstant = 6;
				static const uint32_t kInterpreterState_Error_TypeConversionFailure = 7;
				
				// Flow control modes
				
				static const uint32_t kFlowControlMode_ControlMethod = 0;
				
				static const uint32_t kFlowControlMode_WhileLoop = 1;
				
				static const uint32_t kFlowControlMode_ArgumentEval = 2;
				
				typedef struct
				{
					
					uint32_t CurrentOffset;
					uint32_t MaxOffset;
					
					bool IntegerSizeIs64;
					
				} MethodInvocationContext;
				
				typedef struct
				{
					
					uint8_t Type;
					uint32_t Code;
					
					union
					{
						
						uint32_t IntArg32;
						uint32_t IntArg64;
						
					};
					
					bool ArgIs64Bit;
					
				} FatalResult;
				
				typedef struct
				{
					
					uint32_t OpcodeOffset;
					
					uint32_t ResolvedArumentCount;
					
					uint8_t Opcode;
					bool ExtOpcode;
					
				} AgrumentResolutionBackReference;
				
				typedef struct
				{
					
					DefinitionBlock Block;
					
					MethodInvocationContext MethodContextStack [ kMaxInvocationContextStack ];
					uint32_t MethodContextStackIndex;
					
					uint32_t FlowControlModeStack [ kMaxFlowControlModeStack ];
					uint32_t FlowControlModeStackIndex;
					
					uint32_t State;
					
					FatalResult FatalError;
					
				} InterpreterContext;
				
				static void ( * OpTable [ 0x100 ] ) ( InterpreterContext * Context );
				static void ( * ExtOpTable [ 0x100 ] ) ( InterpreterContext * Context );
				
				static void Exec ( InterpreterContext * Context, uint32_t * Status );
				
				static void EvaluateTermArg ( InterpreterContext * Context, Object :: ACPIObject * Result );
				
				static void IllegalOp ( InterpreterContext * Context );
				
				static void ExtPrefixOp ( InterpreterContext * Context );
				
				static void BreakOp ( InterpreterContext * Context );
				static void BreakPointOp ( InterpreterContext * Context );
				static void ContinueOp ( InterpreterContext * Context );
				static void ElseOp ( InterpreterContext * Context );
				static void FatalOp ( InterpreterContext * Context );
				static void IfOp ( InterpreterContext * Context );
				static void LoadOp ( InterpreterContext * Context );
				static void NoOp ( InterpreterContext * Context );
				static void NotifyOp ( InterpreterContext * Context );
				static void ReleaseOp ( InterpreterContext * Context );
				static void ResetOp ( InterpreterContext * Context );
				static void ReturnOp ( InterpreterContext * Context );
				static void SignalOp ( InterpreterContext * Context );
				static void SleepOp ( InterpreterContext * Context );
				static void StallOp ( InterpreterContext * Context );
				static void UnloadOp ( InterpreterContext * Context );
				static void WhileOp ( InterpreterContext * Context );
				static void AcquireOp ( InterpreterContext * Context );
				static void AddOp ( InterpreterContext * Context );
				static void AndOp ( InterpreterContext * Context );
				static void BufferOp ( InterpreterContext * Context );
				static void ConcatOp ( InterpreterContext * Context );
				static void ConcatResOp ( InterpreterContext * Context );
				static void CondRefOfOp ( InterpreterContext * Context );
				static void CopyObjectOp ( InterpreterContext * Context );
				static void DecrementOp ( InterpreterContext * Context );
				static void DerefOfOp ( InterpreterContext * Context );
				static void DivideOp ( InterpreterContext * Context );
				static void FindSetLeftBitOp ( InterpreterContext * Context );
				static void FindSetRightBitOp ( InterpreterContext * Context );
				static void FromBCDOp ( InterpreterContext * Context );
				static void IncrementOp ( InterpreterContext * Context );
				static void IndexOp ( InterpreterContext * Context );
				static void LAndOp ( InterpreterContext * Context );
				static void LEqualOp ( InterpreterContext * Context );
				static void LGreaterOp ( InterpreterContext * Context );
				static void LLessOp ( InterpreterContext * Context );
				static void LNotOp ( InterpreterContext * Context );
				static void LoadTableOp ( InterpreterContext * Context );
				static void LOrOp ( InterpreterContext * Context );
				static void MatchOp ( InterpreterContext * Context );
				static void MidOp ( InterpreterContext * Context );
				static void ModOp ( InterpreterContext * Context );
				static void MultiplyOp ( InterpreterContext * Context );
				static void NandOp ( InterpreterContext * Context );
				static void NorOp ( InterpreterContext * Context );
				static void NotOp ( InterpreterContext * Context );
				static void ObjectTypeOp ( InterpreterContext * Context );
				static void OrOp ( InterpreterContext * Context );
				static void PackageOp ( InterpreterContext * Context );
				static void VarPackageOp ( InterpreterContext * Context );
				static void RefOfOp ( InterpreterContext * Context );
				static void ShiftLeftOp ( InterpreterContext * Context );
				static void ShiftRightOp ( InterpreterContext * Context );
				static void SizeOfOp ( InterpreterContext * Context );
				static void StoreOp ( InterpreterContext * Context );
				static void SubtractOp ( InterpreterContext * Context );
				static void TimerOp ( InterpreterContext * Context );
				static void ToBCDOp ( InterpreterContext * Context );
				static void ToBufferOp ( InterpreterContext * Context );
				static void ToDecimalStringOp ( InterpreterContext * Context );
				static void ToHexStringOp ( InterpreterContext * Context );
				static void ToIntegerOp ( InterpreterContext * Context );
				static void ToStringOp ( InterpreterContext * Context );
				static void WaitOp ( InterpreterContext * Context );
				static void XorOp ( InterpreterContext * Context );
				
			};
			
		};
		
	};
	
};

#endif
