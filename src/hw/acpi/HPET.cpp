#include <hw/acpi/HPET.h>

#include <hw/acpi/RSDP.h>
#include <hw/acpi/RSDT.h>
#include <hw/acpi/XSDT.h>

#include <mm/KMalloc.h>
#include <mm/KVMap.h>

#include <system/func/KPrintF.h>

const char * HW::ACPI::HPET :: kTableSignature = "HPET";

bool HW::ACPI::HPET :: Validated = false;

HW::ACPI::HPET :: HPETInfo * HW::ACPI::HPET :: HPETs = NULL;
uint32_t HW::ACPI::HPET :: HPETCount = 0;

void HW::ACPI::HPET :: Init ( uint32_t * Status )
{
	
	bool ExtendedACPI;
	
	if ( ! RSDP :: Found () )
	{
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return;
		
	}
	
	if ( RSDP :: GetACPIRevision () == RSDP :: kACPI_Revision_1 )
	{
		
		if ( ! RSDT :: Valid () )
		{
			
			* Status = kACPIStatus_Failure_InvalidTable;
			
			return;
			
		}
		
		ExtendedACPI = false;
			
	}
	else
	{
		
		if ( ! XSDT :: Valid () )
		{
			
			* Status = kACPIStatus_Failure_InvalidTable;
			
			return;
			
		}
		
		ExtendedACPI = true;
		
	}
	
	uint32_t ACPIHPETCount = ( ExtendedACPI ? XSDT :: CountTables ( kTableSignature ) : RSDT :: CountTables ( kTableSignature ) );
	
	if ( ACPIHPETCount == 0 )
	{
		
		* Status = kACPIStatus_Failure_ResourceNotFound;
		
		return;
		
	}
	
	void * PhysicalAddresses [ ACPIHPETCount ];
	
	ExtendedACPI ? XSDT :: FindTables ( kTableSignature, PhysicalAddresses, ACPIHPETCount ) : RSDT :: FindTables ( kTableSignature, PhysicalAddresses, ACPIHPETCount );
	
	uint32_t I;
	uint32_t J;
	uint32_t RealHPETCount = 0;
	
	for ( I = 0; I < ACPIHPETCount; I ++ )
	{
		
		if ( PhysicalAddresses [ I ] != NULL )
			RealHPETCount ++;
		
	}
	
	if ( RealHPETCount == 0 )
	{
		
		* Status = kACPIStatus_Failure_ResourceNotFound;
		
		return;
		
	}
	
	HPETs = reinterpret_cast <HPETInfo *> ( mm_kmalloc ( sizeof ( HPETInfo * ) * RealHPETCount ) );
	
	if ( HPETs == NULL )
	{
		
		* Status = kACPIStatus_Failure_System_OutOfMemory;
		
		return;
		
	}
	
	uint32_t MappedLength = 0x2000;
	HPETTable * TableVirtual = NULL;
	
	for ( J = 0; J < ACPIHPETCount; J ++ )
		if ( PhysicalAddresses [ J ] != NULL )
			break;
	
	TableVirtual = reinterpret_cast <HPETTable *> ( mm_kvmap ( PhysicalAddresses [ J ], 0x2000, MM::Paging::PageTable :: Flags_Writeable ) );
	
	if ( TableVirtual == NULL )
	{
		
		mm_kfree ( HPETs );
		
		* Status = kACPIStatus_Failure_System_OutOfMemory;
		
		return;
		
	}
	
	HPETCount ++;
	
	bool Continue = true;
	
	while ( Continue )
	{
		
		if ( TableVirtual -> Header.Length > MappedLength )
		{
			
			MappedLength = ( TableVirtual -> Header.Length + 0xFFF ) & 0xFFFFF000;
			
			mm_kvunmap ( TableVirtual );
			TableVirtual = reinterpret_cast <HPETTable *> ( mm_kvmap ( PhysicalAddresses [ J ], MappedLength, MM::Paging::PageTable :: Flags_Writeable ) );
			
			if ( TableVirtual == NULL )
			{
				
				mm_kfree ( HPETs );
				
				* Status = kACPIStatus_Failure_System_OutOfMemory;
				
				return;
				
			}
			
			if ( ACPITable :: VerifyTable ( & TableVirtual -> Header ) )
			{
				
				if ( ( TableVirtual -> TimerBlockAddress.AddressSpaceID == kACPIAddress_AddressSpaceID_Memory ) || ( TableVirtual -> TimerBlockAddress.AddressSpaceID == kACPIAddress_AddressSpaceID_SystemIO ) )
				{
					
					HPETs [ HPETCount ].Address = TableVirtual -> TimerBlockAddress.Address;
					HPETs [ HPETCount ].AddressSpaceID = TableVirtual -> TimerBlockAddress.AddressSpaceID;
					HPETs [ HPETCount ].Index = TableVirtual -> HPETNumber;
					HPETs [ HPETCount ].MinimumClockTick = TableVirtual -> MinimumClockTick;
					HPETs [ HPETCount ].ComparratorCount = TableVirtual -> ComparratorCount;
					HPETs [ HPETCount ].AllocationBitmap = 0;
					HPETs [ HPETCount ].WideCounter = TableVirtual -> CounterSize;
					HPETs [ HPETCount ].LegacyIRQ = TableVirtual -> LegacyReplacementCapable;
					
					system_func_kprintf ( "HPET %u: [ Timers: %u, %s-bit ] \n", TableVirtual -> HPETNumber, TableVirtual -> ComparratorCount, TableVirtual -> CounterSize ? "64" : "32" );
					
					HPETCount ++;
					
				}
				else
					system_func_kprintf ( "A" );
				
			}
			else
				system_func_kprintf ( "D" );
			
		}
		
		 J ++;
		
		while ( J < ACPIHPETCount )
		{
			
			if ( PhysicalAddresses [ J ] != NULL )
				break;
			
			 J ++;
			
		}
		
		if ( J == ACPIHPETCount )
			Continue = false;
		
		mm_kvremap ( TableVirtual, PhysicalAddresses [ J ], MM::Paging::PageTable :: Flags_Writeable );
		
	}
	
	if ( HPETCount > 0 )
	{
		
		Validated = true;
		
		* Status = kACPIStatus_Success;
		
	}
	else
		* Status = kACPIStatus_Failure_ResourceNotFound;
	
};

bool HW::ACPI::HPET :: Valid ()
{
	
	return Validated;
	
};

uint32_t HW::ACPI::HPET :: GetHPETCount ()
{
	
	return HPETCount;
	
};

void * HW::ACPI::HPET :: GetHPETHandle ( uint32_t Index )
{
	
	if ( Index < HPETCount )
		return reinterpret_cast <void *> ( & HPETs [ Index ] );
	
	return NULL;
	
};



bool HW::ACPI::HPET :: AllocCounter ( uint32_t GlobalInterrupt, HPETCounterHandle * CounterHandle, uint32_t Requirements )
{
	
	(void) GlobalInterrupt;
	(void) CounterHandle;
	(void) Requirements;
	
	return false;
	
};

void HW::ACPI::HPET :: FreeCounter ( HPETCounterHandle * CounterHandle )
{
	
	(void) CounterHandle;
	
};
			
void HW::ACPI::HPET :: SetCounter32Bits ( HPETCounterHandle * CounterHandle )
{
	
	(void) CounterHandle;
	
};

void HW::ACPI::HPET :: SetCounter64Bits ( HPETCounterHandle * CounterHandle )
{
	
	(void) CounterHandle;
	
};
