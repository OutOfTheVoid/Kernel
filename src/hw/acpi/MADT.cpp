#include <hw/acpi/MADT.h>
#include <hw/acpi/RSDT.h>
#include <hw/acpi/XSDT.h>
#include <hw/acpi/RSDP.h>

#include <mm/KVMap.h>
#include <mm/KMalloc.h>
#include <mm/paging/PageTable.h>

#include <mt/synchronization/Spinlock.h>

#include <KernelDef.h>

bool HW::ACPI::MADT :: Validated = false;
HW::ACPI::MADT :: MADTable * HW::ACPI::MADT :: Table = NULL;

const char * HW::ACPI::MADT :: kSearchString = "APIC";

Vector <HW::ACPI::MADT :: ProcessorLAPICRecord *> * HW::ACPI::MADT :: ProcessorLAPICRecords;
Vector <HW::ACPI::MADT :: IOAPICRecord *> * HW::ACPI::MADT :: IOAPICRecords;
Vector <HW::ACPI::MADT :: InterruptSourceOverrideRecord *> * HW::ACPI::MADT :: InterruptSourceOverrideRecords;

MT::Synchronization::Spinlock :: Spinlock_t HW::ACPI::MADT :: Lock = MT::Synchronization::Spinlock :: Initializer ();

void HW::ACPI::MADT :: Init ()
{
	
	uint32_t TableLength;
	void * PhysAddr;
	
	RecordHeader * RecordBase;
	
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
	
	if ( Validated )
		return;
	
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
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
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

