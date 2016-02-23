#ifndef HW_ACPI_AML_OBJECT_H
#define HW_ACPI_AML_OBJECT_H

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
			
			typedef struct
			{
				
				const char * CString;
				
				uint32_t Length;
				
				bool Allocated;
				
			} ACPIString;
			
			class Object
			{
			public:
				
				static const uint32_t kObjectType_Uninitialized = 0;
				static const uint32_t kObjectType_Int32 = 1;
				static const uint32_t kObjectType_Int64 = 2;
				static const uint32_t kObjectType_BCDInt32 = 3;
				static const uint32_t kObjectType_BCDInt64 = 4;
				static const uint32_t kObjectType_String = 5;
				static const uint32_t kObjectType_Buffer = 6;
				static const uint32_t kObjectType_BufferField = 7;
				static const uint32_t kObjectType_DefinitionBlockHandle = 8;
				static const uint32_t kObjectType_DebugObject = 9;
				static const uint32_t kObjectType_Device = 10;
				static const uint32_t kObjectType_Event = 11;
				static const uint32_t kObjectType_FieldUnit = 12;
				static const uint32_t kObjectType_Int32Const = 13;
				static const uint32_t kObjectType_Int64Const = 14;
				static const uint32_t kObjectType_ControlMethod = 15;
				static const uint32_t kObjectType_Mutex = 16;
				static const uint32_t kObjectType_ObjectReference = 17;
				static const uint32_t kObjectType_OperatingRegion = 18;
				static const uint32_t kObjectType_Package = 19;
				static const uint32_t kObjectType_PowerSource = 10;
				static const uint32_t kObjectType_Processor = 11;
				static const uint32_t kObjectType_RawDataBuffer = 12;
				static const uint32_t kObjectType_ThermalZone = 13;	
				
				typedef struct
				{
					
					uint32_t Type;
					
					union
					{
						
						uint32_t Int32;
						uint64_t Int64;
						
						uint32_t BCDInt32;
						uint64_t BCDInt64;
						
						ACPIString String;
						
					// Buffer Buff;
					// BufferField BField;
						
					// DeffBlockHandle DBHandle;
					// Package Pack;
						
					// DebugObj Debug;
						
					// Device Dev;
					// PowerResource PowerRes;
					// ThermalZone TZone;
						
					// Event Evt;
						
					// ControlMethod Method;
						
					// Mutex Mut;
						
					// ObectReference ObjRef;
						
					// Processor Proc;
						
					// RawDataBuffer RawBuff;
					// FieldUnit FUnit
						
					} Value;
					
				} ACPIObject;
				
				static bool CoerceObjectToInteger ( ACPIObject * Object, bool Is64Bit );
				
			};
			
		};
		
	};
	
};

#endif
