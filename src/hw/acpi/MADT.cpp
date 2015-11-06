#include <hw/acpi/MADT.h>
#include <hw/acpi/RSDT.h>
#include <hw/acpi/XSDT.h>
#include <hw/acpi/RSDP.h>

#include <mm/KVMap.h>
#include <mm/KMalloc.h>
#include <mm/paging/PageTable.h>

#include <cpputil/Memory.h>

#include <KernelDef.h>

bool HW::ACPI::MADT :: Validated = false;
HW::ACPI::MADT :: MADTable * HW::ACPI::MADT :: Table = NULL;

const char * HW::ACPI::MADT :: kSearchString = "APIC";

Vector <HW::ACPI::MADT :: ProcessorLAPICRecord *> * HW::ACPI::MADT :: ProcessorLAPICRecords = NULL;
Vector <HW::ACPI::MADT :: IOAPICRecord *> * HW::ACPI::MADT :: IOAPICRecords = NULL;
Vector <HW::ACPI::MADT :: InterruptSourceOverrideRecord *> * HW::ACPI::MADT :: InterruptSourceOverrideRecords = NULL;

void HW::ACPI::MADT :: Init ()
{
	
	uint32_t TableLength;
	void * PhysAddr;
	
	RecordHeader * RecordBase;
	
	if ( Validated || ! RSDP :: Found () )
		return;
	
	ProcessorLAPICRecords = new Vector <ProcessorLAPICRecord *> ();
	
	if ( ProcessorLAPICRecords == NULL )
		return;
	
	IOAPICRecords = new Vector <IOAPICRecord *> ();
	
	if ( ProcessorLAPICRecords == NULL )
	{
		
		delete ProcessorLAPICRecords;
		
		return;
		
	}
	
	InterruptSourceOverrideRecords = new Vector <InterruptSourceOverrideRecord *> ();
	
	if ( InterruptSourceOverrideRecords == NULL )
	{
		
		delete ProcessorLAPICRecords;
		delete IOAPICRecords;
		
		return;
		
	}
	
	if ( RSDP :: GetACPIRevision () == RSDP :: kACPI_Revision_1 )
	{
		
		if ( ! RSDT :: Valid () )
		{
			
			delete ProcessorLAPICRecords;
			delete IOAPICRecords;
			delete InterruptSourceOverrideRecords;
			
			return;
			
		}
		
		PhysAddr = RSDT :: FindTable ( kSearchString );
		
	}
	else
	{
		
		if ( ! XSDT :: Valid () )
		{
			
			delete ProcessorLAPICRecords;
			delete IOAPICRecords;
			delete InterruptSourceOverrideRecords;
			
			return;
			
		}
		
		PhysAddr = XSDT :: FindTable ( kSearchString );
		
	}
	
	if ( PhysAddr == NULL )
	{
		
		delete ProcessorLAPICRecords;
		delete IOAPICRecords;
		delete InterruptSourceOverrideRecords;
		
		return;
		
	}
	
	Table = reinterpret_cast <MADTable *> ( mm_kvmap ( PhysAddr, 0x2000, MM::Paging::PageTable :: Flags_Writeable ) );
	
	if ( Table == NULL )
	{
		
		delete ProcessorLAPICRecords;
		delete IOAPICRecords;
		delete InterruptSourceOverrideRecords;
		
		return;
		
	}
	
	TableLength = ( reinterpret_cast <uint32_t> ( PhysAddr ) + Table -> Header.Length ) - ( reinterpret_cast <uint32_t> ( PhysAddr ) & 0xFFFFF000 );
	
	if ( TableLength > 0x2000 )
	{
		
		mm_kvunmap ( Table );
		Table = reinterpret_cast <MADTable *> ( mm_kvmap ( PhysAddr, TableLength, MM::Paging::PageTable :: Flags_Writeable ) );
		
		if ( Table == NULL )
		{
			
			delete ProcessorLAPICRecords;
			delete IOAPICRecords;
			delete InterruptSourceOverrideRecords;
			
			return;
			
		}
		
	}
	
	if ( ! ACPITable :: VerifyTable ( & Table -> Header ) )
	{
		
		mm_kvunmap ( Table );
		
		delete ProcessorLAPICRecords;
		delete IOAPICRecords;
		delete InterruptSourceOverrideRecords;
		
		return;
		
	}
	
	RecordBase = reinterpret_cast <RecordHeader *> ( reinterpret_cast <uint32_t> ( Table ) + sizeof ( MADTable ) );
	
	ProcessorLAPICRecord * PLRRecord;
	IOAPICRecord * IOARecord;
	InterruptSourceOverrideRecord * ISORecord;
	
	while ( reinterpret_cast <uint32_t> ( RecordBase ) < reinterpret_cast <uint32_t> ( Table ) + Table -> Header.Length )
	{
		
		switch ( RecordBase -> Type )
		{
			
		case kRecordType_ProcessorLAPICRecord:
			
			PLRRecord = reinterpret_cast <ProcessorLAPICRecord *> ( RecordBase );
			
			ProcessorLAPICRecords -> Push ( PLRRecord );
			
			break;
			
		case kRecordType_IOAPICRecord:
			
			IOARecord = reinterpret_cast <IOAPICRecord *> ( RecordBase );
			
			IOAPICRecords -> Push ( IOARecord );
			
			break;
			
		case kRecordType_InterruptSourceOverride:
			
			ISORecord = reinterpret_cast <InterruptSourceOverrideRecord *> ( RecordBase );
			
			InterruptSourceOverrideRecords -> Push ( ISORecord );
			
			break;
			
		}
		
		RecordBase = reinterpret_cast <RecordHeader *> ( reinterpret_cast <uint32_t> ( RecordBase ) + RecordBase -> Length );
		
	}
	
	Validated = true;
	
};

bool HW::ACPI::MADT :: Valid ()
{
	
	return Validated;
	
};

void HW::ACPI::MADT :: Discard ()
{
	
	if ( ! Validated )
		return;
	
	mm_kvunmap ( Table );
	
	delete ProcessorLAPICRecords;
	delete IOAPICRecords;
	delete InterruptSourceOverrideRecords;
	
	Table = NULL;
	
};

uint32_t HW::ACPI::MADT :: GetAPICBaseAddress ()
{
	
	if ( ! Validated )
		return 0;
	
	return Table -> LAPICAddress;
	
};

uint32_t HW::ACPI::MADT :: GetProcessorCount ()
{
	
	if ( ! Validated )
		return 0;
	
	return ProcessorLAPICRecords -> Length ();
	
};

uint8_t HW::ACPI::MADT :: GetProcessorLAPICID ( uint32_t Index )
{
	
	if ( ! Validated )
		return 0;
	
	return ( * ProcessorLAPICRecords ) [ Index ] -> APICID;
	
};

uint8_t HW::ACPI::MADT :: GetProcessorID ( uint32_t Index )
{
	
	if ( ! Validated )
		return 0;
	
	return ( * ProcessorLAPICRecords ) [ Index ] -> APICProcessorID;
	
};

bool HW::ACPI::MADT :: GetProcessorEnabled ( uint32_t Index )
{
	
	if ( ! Validated )
		return false;
	
	return ( ( * ProcessorLAPICRecords ) [ Index ] -> Flags & kAPICFlags_ProcessorEnabled );
	
};

uint32_t HW::ACPI::MADT :: GetIOAPICCount ()
{
	
	if ( ! Validated )
		return 0;
	
	return IOAPICRecords -> Length ();
	
};

uint32_t HW::ACPI::MADT :: GetIOAPICBaseAddress ( uint32_t Index )
{
	
	if ( ! Validated )
		return 0;
	
	return ( * IOAPICRecords ) [ Index ] -> Address;
	
};

uint8_t HW::ACPI::MADT :: GetIOAPICID ( uint32_t Index )
{
	
	if ( ! Validated )
		return 0;
	
	return ( * IOAPICRecords ) [ Index ] -> ID;
	
};

uint32_t HW::ACPI::MADT :: GetIOAPICGlobalSystemInterruptBase ( uint32_t Index )
{
	
	if ( ! Validated )
		return 0;
	
	return ( * IOAPICRecords ) [ Index ] -> GlobalSystemInterruptBase;
	
};

uint32_t HW::ACPI::MADT :: GetInterruptSourceOverrideCount ()
{
	
	if ( ! Validated )
		return 0;
	
	return InterruptSourceOverrideRecords -> Length ();
	
};

uint8_t HW::ACPI::MADT :: GetInterruptSourceOverrideBus ( uint32_t Index )
{
	
	if ( ! Validated )
		return 0;
	
	return ( * InterruptSourceOverrideRecords ) [ Index ] -> Bus;
	
};

uint8_t HW::ACPI::MADT :: GetInterruptSourceOverrideSourceIRQ ( uint32_t Index )
{
	
	if ( ! Validated )
		return 0;
	
	return ( * InterruptSourceOverrideRecords ) [ Index ] -> Source;
	
};

uint32_t HW::ACPI::MADT :: GetInterruptSourceOverrideInterrupt ( uint32_t Index )
{
	
	if ( ! Validated )
		return 0;
	
	return ( * InterruptSourceOverrideRecords ) [ Index ] -> GlobalSystemInterrupt;
	
};

uint16_t HW::ACPI::MADT :: GetInterruptSourceOverrideFlags ( uint32_t Index )
{
	
	if ( ! Validated )
		return 0;
	
	return ( * InterruptSourceOverrideRecords ) [ Index ] -> Flags;
	
};
