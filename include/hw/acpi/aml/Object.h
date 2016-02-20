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
			
			const uint32_t kObjectType_Uninitialized = 0;
			const uint32_t kObjectType_Int32 = 1;
			const uint32_t kObjectType_Int64 = 2;
			const uint32_t kObjectType_BCDInt32 = 3;
			const uint32_t kObjectType_BCDInt64 = 4;
			const uint32_t kObjectType_String = 5;
			const uint32_t kObjectType_Buffer = 6;
			const uint32_t kObjectType_BufferField = 7;
			const uint32_t kObjectType_DefinitionBlockHandle = 8;
			const uint32_t kObjectType_DebugObject = 9;
			const uint32_t kObjectType_Device = 10;
			const uint32_t kObjectType_Event = 11;
			const uint32_t kObjectType_FieldUnit = 12;
			const uint32_t kObjectType_Int32Const = 13;
			const uint32_t kObjectType_Int64Const = 14;
			const uint32_t kObjectType_ControlMethod = 15;
			const uint32_t kObjectType_Mutex = 16;
			const uint32_t kObjectType_ObjectReference = 17;
			const uint32_t kObjectType_OperatingRegion = 18;
			const uint32_t kObjectType_Package = 19;
			const uint32_t kObjectType_PowerSource = 10;
			const uint32_t kObjectType_Processor = 11;
			const uint32_t kObjectType_RawDataBuffer = 12;
			const uint32_t kObjectType_ThermalZone = 13;
			
			typedef struct
			{
				
				uint32_t Type;
				
				union
				{
					
					uint32_t Int32;
					uint64_t Int64;
					
					uint32_t BCDInt32;
					uint64_t BCDInt64;
					
					const char * String;
					
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
			
		};
		
	};
	
};

#endif
