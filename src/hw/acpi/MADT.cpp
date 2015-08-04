#include <hw/acpi/MADT.h>
#include <hw/acpi/RSDT.h>
#include <hw/acpi/XSDT.h>
#include <hw/acpi/RSDP.h>

#include <mm/KVMap.h>
#include <mm/KMalloc.h>
#include <mm/paging/PageTable.h>

#include <KernelDef.h>

bool HW::ACPI::MADT :: Validated = false;
HW::ACPI::MADT :: MADTable * HW::ACPI::MADT :: Table = NULL;

const char * HW::ACPI::MADT :: kSearchString = "APIC";

Vector <HW::ACPI::MADT :: ProcessorLAPICRecord *> * HW::ACPI::MADT :: ProcessorLAPICRecords = NULL;
Vector <HW::ACPI::MADT :: IOAPICRecord *> * HW::ACPI::MADT :: IOAPICRecords = NULL;
Vector <HW::ACPI::MADT :: InterruptSourceOverrideRecord *> * HW::ACPI::MADT :: InterruptSourceOverrideRecords = NULL;

MT::Synchronization::Spinlock :: Spinlock_t HW::ACPI::MADT :: Lock = MT::Synchronization::Spinlock :: Initializer ();

void HW::ACPI::MADT :: Init ()
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	uint32_t TableLength;
	void * PhysAddr;
	
	RecordHeader * RecordBase;
	
	if ( Validated || ! RSDP :: Found () )
	{
		
		MT::Synchronization::Spinlock :: Release ( & Lock );
			
		return;
	
	}
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
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
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	Validated = true;
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
};

bool HW::ACPI::MADT :: Valid ()
{
	
	bool IsValid = Validated;
	
	return IsValid;
	
};

void HW::ACPI::MADT :: Discard ()
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	if ( ! Validated )
	{
		
		MT::Synchronization::Spinlock :: Release ( & Lock );
		
		return;
		
	}
	
	mm_kvunmap ( Table );
	
	delete ProcessorLAPICRecords;
	delete IOAPICRecords;
	delete InterruptSourceOverrideRecords;
	
	Table = NULL;
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
};

uint32_t HW::ACPI::MADT :: GetAPICBaseAddress ()
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	if ( ! Validated )
	{
		
		MT::Synchronization::Spinlock :: Release ( & Lock );
		
		return 0;
		
	}
	
	uint32_t Address = Table -> LAPICAddress;
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
	return Address;
	
};

uint32_t HW::ACPI::MADT :: GetProcessorCount ()
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	if ( ! Validated )
	{
		
		MT::Synchronization::Spinlock :: Release ( & Lock );
		
		return 0;
		
	}
	
	uint32_t Count = ProcessorLAPICRecords -> Length ();
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
	return Count;
	
};

uint8_t HW::ACPI::MADT :: GetProcessorLAPICID ( uint32_t Index )
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	if ( ! Validated )
	{
		
		MT::Synchronization::Spinlock :: Release ( & Lock );
		
		return 0;
		
	}
	
	uint8_t ID = ( * ProcessorLAPICRecords ) [ Index ] -> APICID;
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
	return ID;
	
};

uint8_t HW::ACPI::MADT :: GetProcessorID ( uint32_t Index )
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	if ( ! Validated )
	{
		
		MT::Synchronization::Spinlock :: Release ( & Lock );
		
		return 0;
		
	}
	
	uint8_t ID = ( * ProcessorLAPICRecords ) [ Index ] -> APICProcessorID;
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
	return ID;
	
};

bool HW::ACPI::MADT :: GetProcessorEnabled ( uint32_t Index )
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	if ( ! Validated )
	{
		
		MT::Synchronization::Spinlock :: Release ( & Lock );
		
		return false;
		
	}
	
	bool Enabled = ( ( * ProcessorLAPICRecords ) [ Index ] -> Flags & kAPICFlags_ProcessorEnabled );
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
	return Enabled;
	
};

uint32_t HW::ACPI::MADT :: GetIOAPICCount ()
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	if ( ! Validated )
	{
		
		MT::Synchronization::Spinlock :: Release ( & Lock );
		
		return 0;
		
	}
	
	uint32_t Count = IOAPICRecords -> Length ();
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
	return Count;
	
};

uint32_t HW::ACPI::MADT :: GetIOAPICBaseAddress ( uint32_t Index )
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	if ( ! Validated )
	{
		
		MT::Synchronization::Spinlock :: Release ( & Lock );
		
		return 0;
		
	}
	
	uint32_t Address = ( * IOAPICRecords ) [ Index ] -> Address;
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
	return Address;
	
};

uint8_t HW::ACPI::MADT :: GetIOAPICID ( uint32_t Index )
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	if ( ! Validated )
	{
		
		MT::Synchronization::Spinlock :: Release ( & Lock );
		
		return 0;
		
	}
	
	uint8_t ID = ( * IOAPICRecords ) [ Index ] -> ID;
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
	return ID;
	
};

uint32_t HW::ACPI::MADT :: GetIOAPICGlobalSystemInterruptBase ( uint32_t Index )
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	if ( ! Validated )
	{
		
		MT::Synchronization::Spinlock :: Release ( & Lock );
		
		return 0;
		
	}
	
	uint32_t InterruptBase = ( * IOAPICRecords ) [ Index ] -> GlobalSystemInterruptBase;
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
	return InterruptBase;
	
};

uint32_t HW::ACPI::MADT :: GetInterruptSourceOverrideCount ()
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	if ( ! Validated )
	{
		
		MT::Synchronization::Spinlock :: Release ( & Lock );
		
		return 0;
		
	}
	
	uint32_t Count = InterruptSourceOverrideRecords -> Length ();
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
	return Count;
	
};

uint8_t HW::ACPI::MADT :: GetInterruptSourceOverrideBus ( uint32_t Index )
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	if ( ! Validated )
	{
		
		MT::Synchronization::Spinlock :: Release ( & Lock );
		
		return 0;
		
	}
	
	uint8_t Bus = ( * InterruptSourceOverrideRecords ) [ Index ] -> Bus;
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
	return Bus;
	
};

uint8_t HW::ACPI::MADT :: GetInterruptSourceOverrideSourceIRQ ( uint32_t Index )
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	if ( ! Validated )
	{
		
		MT::Synchronization::Spinlock :: Release ( & Lock );
		
		return 0;
		
	}
	
	uint8_t Source = ( * InterruptSourceOverrideRecords ) [ Index ] -> Source;
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
	return Source;
	
};

uint32_t HW::ACPI::MADT :: GetInterruptSourceOverrideInterrupt ( uint32_t Index )
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	if ( ! Validated )
	{
		
		MT::Synchronization::Spinlock :: Release ( & Lock );
		
		return 0;
		
	}
	
	uint32_t GlobalSystemInterrupt = ( * InterruptSourceOverrideRecords ) [ Index ] -> GlobalSystemInterrupt;
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
	return GlobalSystemInterrupt;
	
};

uint16_t HW::ACPI::MADT :: GetInterruptSourceOverrideFlags ( uint32_t Index )
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	if ( ! Validated )
	{
		
		MT::Synchronization::Spinlock :: Release ( & Lock );
		
		return 0;
		
	}
	
	uint16_t Flags = ( * InterruptSourceOverrideRecords ) [ Index ] -> Flags;
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
	return Flags;
	
};
