#include <hw/acpi/DSDT.h>
#include <hw/acpi/FADT.h>

#include <mm/KVMap.h>
#include <mm/paging/PageTable.h>

bool HW::ACPI::DSDT :: Validated = false;

HW::ACPI::DSDT :: DSDTable * HW::ACPI::DSDT :: Table = NULL;
uint32_t HW::ACPI::DSDT :: TableLength = 0;

void HW::ACPI::DSDT :: Init ( uint32_t * Status )
{
	
	uint32_t SubStatus;
	
	if ( Validated )
	{
		
		* Status = kACPIStatus_Success;
		
		return;
		
	}
	
	void * PhysAddr = FADT :: GetDSDTAddress ( & SubStatus );
	
	if ( SubStatus != kACPIStatus_Success )
	{
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return;
		
	}
	
	if ( PhysAddr == NULL )
	{
		
		* Status = kACPIStatus_Failure_ResourceNotFound;
		
		return;
		
	}
	
	Table = reinterpret_cast <DSDTable *> ( mm_kvmap ( PhysAddr, 0x2000, 0 ) );
	
	if ( Table == NULL )
	{
		
		* Status = kACPIStatus_Failure_System_OutOfMemory;
		
		return;
		
	}
	
	TableLength = ( reinterpret_cast <uint32_t> ( PhysAddr ) + Table -> Header.Length ) - ( reinterpret_cast <uint32_t> ( PhysAddr ) & 0xFFFFF000 );
	
	if ( TableLength > 0x2000 )
	{
		
		mm_kvunmap ( Table );
		
		Table = reinterpret_cast <DSDTable *> ( mm_kvmap ( PhysAddr, TableLength, MM::Paging::PageTable :: Flags_Writeable ) );
		
		if ( Table == NULL )
		{
			
			* Status = kACPIStatus_Failure_System_OutOfMemory;
			
			return;
			
		}
		
	}
	
	Validated = true;
	
	* Status = kACPIStatus_Success;
	
};

bool HW::ACPI::DSDT :: Valid ()
{
	
	return Validated;
	
};

void * HW::ACPI::DSDT :: GetDefinitionBlockAddress ( uint32_t * Status )
{
	
	if ( ! Validated )
	{
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return NULL;
		
	}
	
	void * Address = & ( Table -> DefinitionBlockStart [ 0 ] );
	
	* Status = kACPIStatus_Success;
	
	return Address;
	
};

uint32_t HW::ACPI::DSDT :: GetDefinitionBlockLength ( uint32_t * Status )
{
	
	if ( ! Validated )
	{
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return 0;
		
	}
	
	uint32_t Length = Table -> Header.Length - reinterpret_cast <uint32_t> ( sizeof ( ACPITable :: ACPITableHeader ) );
	
	* Status = kACPIStatus_Success;
	
	return Length;
	
};
