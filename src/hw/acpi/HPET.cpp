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
			
		}
			
		if ( ACPITable :: VerifyTable ( & TableVirtual -> Header ) )
		{
			
			if ( TableVirtual -> TimerBlockAddress.AddressSpaceID == kACPIAddress_AddressSpaceID_SystemIO )
			{
				
				HPETs [ HPETCount ].Address = TableVirtual -> TimerBlockAddress.Address;
				HPETs [ HPETCount ].AddressSpaceID = TableVirtual -> TimerBlockAddress.AddressSpaceID;
				HPETs [ HPETCount ].Index = TableVirtual -> HPETNumber;
				HPETs [ HPETCount ].MinimumClockTick = TableVirtual -> MinimumClockTick;
				HPETs [ HPETCount ].ComparratorCount = TableVirtual -> ComparratorCount;
				HPETs [ HPETCount ].AllocationBitmap = 0;
				HPETs [ HPETCount ].WideCounter = TableVirtual -> CounterSize;
				HPETs [ HPETCount ].LegacyIRQ = TableVirtual -> LegacyReplacementCapable;
				
				HPETCount ++;
				
			}
			else if ( TableVirtual -> TimerBlockAddress.AddressSpaceID == kACPIAddress_AddressSpaceID_Memory ) 
			{
				
				void * HPETVirtual = mm_kvmap ( reinterpret_cast <void *> ( TableVirtual -> TimerBlockAddress.Address ), 0x1000, MM::Paging::PageTable :: Flags_Writeable | MM::Paging::PageTable :: Flags_NoCache );
				
				if ( HPETVirtual == NULL )
				{
					
					mm_kfree ( TableVirtual );
					
					for ( uint32_t X = 0; X <= HPETCount; X ++ )
					{
						
						if ( HPETs [ X ].AddressSpaceID == kACPIAddress_AddressSpaceID_Memory )
							mm_kfree ( reinterpret_cast <void *> ( HPETs [ X ].Address ) );
						
					}
					
					mm_kfree ( HPETs );
					
					* Status = kACPIStatus_Failure_System_OutOfMemory;
					
					return;
					
				}
				
				HPETs [ HPETCount ].Address = reinterpret_cast <uint32_t> ( HPETVirtual );
				HPETs [ HPETCount ].AddressSpaceID = TableVirtual -> TimerBlockAddress.AddressSpaceID;
				HPETs [ HPETCount ].Index = TableVirtual -> HPETNumber;
				HPETs [ HPETCount ].MinimumClockTick = TableVirtual -> MinimumClockTick;
				HPETs [ HPETCount ].ComparratorCount = TableVirtual -> ComparratorCount;
				HPETs [ HPETCount ].AllocationBitmap = 0;
				HPETs [ HPETCount ].WideCounter = TableVirtual -> CounterSize;
				HPETs [ HPETCount ].LegacyIRQ = TableVirtual -> LegacyReplacementCapable;
				
				HPETCount ++;
				
			}
			
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

uint32_t HW::ACPI::HPET :: GetHPETCount ( uint32_t * Status, HPETInfo * Info )
{
	
	( void ) Info;
	
	if ( ! Validated )
	{
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return 0;
		
	}
	
	* Status = kACPIStatus_Success;
	
	return HPETCount;
	
};

HW::ACPI::HPET :: HPETInfo * HW::ACPI::HPET :: GetHPET ( uint32_t Index, uint32_t * Status )
{
	
	if ( ! Validated )
	{
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return NULL;
		
	}
	
	if ( Index < HPETCount )
	{
		
		* Status = kACPIStatus_Success;
		
		return & HPETs [ Index ];
		
	}
	
	* Status = kACPIStatus_Failure_ResourceNotFound;
	
	return NULL;
	
};

void HW::ACPI::HPET :: AllocCounter ( uint32_t GlobalInterrupt, HPETCounterInfo * Counter, uint32_t Requirements, uint32_t * Status )
{
	
	if ( Counter == NULL )
	{
		
		* Status = kACPIStatus_Failure_NullArgument;
		
		return;
		
	}
	
	if ( ! Validated )
	{
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return;
		
	}
	
	if ( GlobalInterrupt > 31 )
	{
		
		* Status = kACPIStatus_Failure_ResourceIncompatible;
		
		return;
		
	}
	
	uint32_t I;
	uint32_t J;
	
	for ( I = 0; I < HPETCount; I ++ )
	{
		
		if ( HPETs [ I ].AllocationBitmap != 0xFFFFFFFF )
		{
			
			for ( J = 0; J < HPETs [ I ].ComparratorCount; J ++ )
			{
				
				if ( ( HPETs [ I ].AllocationBitmap & ( 1 << J ) ) == 0 )
				{
					
					uint32_t CapabilitiesLow;
					uint32_t CapabilitiesHigh;
					
					ReadRegister ( HPETs [ I ].Address, HPETs [ I ].AddressSpaceID, kRegister_TimerNConfigurationCapabilitiesBase + I * kRegisterStride_TimerN, & CapabilitiesLow, & CapabilitiesHigh );
					
					if ( Requirements & kRequirement_LegacyReplacement_PIT )
					{
						
						// Todo: Check legacy PIT replacement requirement...
						
					}
					
					if ( Requirements & kRequirement_LegacyReplacement_RTC )
					{
						
						// Todo: Check legacy RTC replacement requirement...
						
					}
					
					if ( Requirements & kRequirement_FSBDelivery )
					{
						
						// Todo: Check FSB Delivery requirement
						
					}
					
					if ( Requirements & kRequirement_Periodic )
					{
						
						if ( ( CapabilitiesLow & kRegisterFlags_TimerNConfigurationCapabilities_Low_Periodic ) == 0 )
							continue;
						
					}
					
					uint32_t InterruptBit = 1 << GlobalInterrupt;
					
					if ( CapabilitiesHigh & InterruptBit )
					{
						
						HPETs [ I ].AllocationBitmap |= ( 1 << J );
						
						Counter -> HPETInfoPointer = & HPETs [ I ];
						Counter -> Counter = J;
						
						Counter -> GlobalInterrupt = GlobalInterrupt;
						Counter -> CapabilitiesLow = CapabilitiesLow;
						Counter -> CapabilitiesHigh = CapabilitiesHigh;
						
						* Status = kACPIStatus_Success;
						
						return;
						
					}
					
				}
				
			}
			
		}
		
	}
	
	* Status = kACPIStatus_Failure_ResourceIncompatible;
	
	return;
	
};

void HW::ACPI::HPET :: FreeCounter ( HPETCounterInfo * Counter, uint32_t * Status )
{
	
	if ( ! Validated )
	{
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return;
		
	}
	
	if ( Counter == NULL )
	{
		
		* Status = kACPIStatus_Failure_NullArgument;
		
		return;
		
	}
	
	if ( Counter -> HPETInfoPointer == NULL )
	{
		
		* Status = kACPIStatus_Failure_NullArgument;
		
		return;
		
	}
	
	Counter -> HPETInfoPointer -> AllocationBitmap &= ~ ( 1 << Counter -> Counter );
	Counter -> HPETInfoPointer = NULL;
	
};
			
void HW::ACPI::HPET :: SetCounter32Bits ( HPETCounterInfo * CounterHandle, uint32_t * Status )
{
	
	(void) Status;
	(void) CounterHandle;
	
};

void HW::ACPI::HPET :: SetCounter64Bits ( HPETCounterInfo * CounterHandle, uint32_t * Status )
{
	
	(void) Status;
	(void) CounterHandle;
	
};
