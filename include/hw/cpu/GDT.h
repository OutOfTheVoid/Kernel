#ifndef HW_CPU_GDT_H
#define HW_CPU_GDT_H

#include <hw/HW.h>
#include <hw/cpu/CPU.h>

#include <stdint.h>

namespace HW
{
	
	namespace CPU
	{
		
		class GDT
		{
		public:
			
			static const uint32_t EntryIndex_GDTNull = 0;
			static const uint32_t EntryIndex_KernelText = 1;
			static const uint32_t EntryIndex_KernelData = 2;
			
			typedef struct
			{
				
				uint16_t Limit;
				uint32_t Base;
				
			} __attribute__((packed)) GDTR;
			
			typedef enum
			{
				
				kFlags_Granularity4KB = 0x04,
				kFlags_Granularity1B = 0x00,
				
				kFlags_16Bit = 0x00,
				kFlags_32Bit = 0x08,
				
				kFlags_None = 0x00,
				
			} Flags;
			
			typedef enum
			{
				
				kAccessType_Present = 0x80,
				kAccessType_Absent = 0x00,
				
				kAccessType_Ring0 = 0x00,
				kAccessType_Ring1 = 0x20,
				kAccessType_Ring2 = 0x40,
				kAccessType_Ring3 = 0x60,
				
				kAccessType_Code = 0x08,
				kAccessType_Data = 0x00,
				
				kAccessType_DirectionUp = 0x00,
				kAccessType_DirectionDown = 0x04,
				kAccessType_ExHigherEqPrivelege = 0x04,
				kAccessType_ExMatchingPrivelege = 0x00,
				
				kAccessType_CodeReadable = 0x02,
				kAccessType_CodeExOnly = 0x00,
				kAccessType_DataWiteable = 0x02,
				kAccessType_DataReadOnly = 0x00,
				
				kAccessType_Bit4 = 0x10,
				
				kAccessType_None = 0x00,
				
			} AccessType;

			static void Init ( uint8_t EntryCount );
			
			static void SetDataEntry32 ( uint32_t Index, uint32_t Base, uint32_t Limit, uint8_t Ring, bool Writeable );
			static void SetCodeEntry32 ( uint32_t Index, uint32_t Base, uint32_t Limit, uint8_t Ring, bool Readable );
			
			static void Swap ();
			
		private:
			
			typedef struct
			{
				
				uint16_t LimitLow;
				uint16_t BaseLow;
				uint8_t BaseMiddle;
				uint8_t Access;
				
				uint8_t LimitHigh : 4;
				uint8_t Flags : 4;
				
				uint8_t BaseHigh;
				
			} __attribute__((packed)) GDTEntry;
			
			static void DefineEntry ( GDTEntry * Entry, uint32_t Limit, uint32_t Base, AccessType Access, Flags Flags );
			
			static GDTEntry * Entries;
			static uint32_t EntryCount;
			
			static bool OwnsCurrent;
			
		};

	};

};

#endif
