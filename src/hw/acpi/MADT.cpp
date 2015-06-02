#include <hw/acpi/MADT.h>
#include <hw/acpi/RSDT.h>
#include <hw/acpi/XSDT.h>
#include <hw/acpi/RSDP.h>

#include <system/func/KPrintf.h>

#include <mm/KVMap.h>
#include <mm/KMalloc.h>
#include <mm/paging/PageTable.h>

#include <KernelDef.h>

bool HW::ACPI::MADT :: Validated = false;
HW::ACPI::MADT :: MADTable * HW::ACPI::MADT :: Table = NULL;

const char * HW::ACPI::MADT :: kSearchString = "APIC";

Vector <HW::ACPI::MADT :: ProcessorLAPICRecord *> * HW::ACPI::MADT :: ProcessorLAPICRecords;
Vector <HW::ACPI::MADT :: IOAPICRecord *> * HW::ACPI::MADT :: IOAPICRecords;
Vector <HW::ACPI::MADT :: InterruptSourceOverride *> * HW::ACPI::MADT :: InterruptSourceOverrideRecords;

void HW::ACPI::MADT :: Init ()
{
	
	uint32_t TableLength;
	void * PhysAddr;
	
	RecordHeader * RecordBase;
	
	ProcessorLAPICRecords = reinterpret_cast <Vector <HW::ACPI::MADT :: ProcessorLAPICRecord *> *> ( mm_kmalloc ( sizeof ( Vector <HW::ACPI::MADT :: ProcessorLAPICRecord *> ) ) );
	
	if ( ProcessorLAPICRecords == NULL )
		return;
	
	IOAPICRecords = reinterpret_cast <Vector <HW::ACPI::MADT :: IOAPICRecord *> *> ( mm_kmalloc ( sizeof ( Vector <HW::ACPI::MADT :: IOAPICRecord *> ) ) );
	
	if ( ProcessorLAPICRecords == NULL )
	{
		
		mm_kfree ( ProcessorLAPICRecords );
		
		return;
		
	}
	
	InterruptSourceOverrideRecords = reinterpret_cast <Vector <HW::ACPI::MADT :: InterruptSourceOverride *> *> ( mm_kmalloc ( sizeof ( Vector <HW::ACPI::MADT :: InterruptSourceOverride *> ) ) );
	
	if ( InterruptSourceOverrideRecords == NULL )
	{
		
		mm_kfree ( ProcessorLAPICRecords );
		mm_kfree ( IOAPICRecords );
		
		return;
		
	}
	
	if ( Validated )
		return;
	
	if ( RSDP :: GetACPIRevision () == RSDP :: kACPI_Revision_1 )
	{
		
		if ( ! RSDT :: Valid () )
		{
			
			mm_kfree ( ProcessorLAPICRecords );
			mm_kfree ( IOAPICRecords );
			mm_kfree ( InterruptSourceOverrideRecords );
			
			return;
			
		}
		
		PhysAddr = RSDT :: FindTable ( kSearchString );
		
	}
	else
	{
		
		if ( ! XSDT :: Valid () )
		{
			
			mm_kfree ( ProcessorLAPICRecords );
			mm_kfree ( IOAPICRecords );
			mm_kfree ( InterruptSourceOverrideRecords );
			
			return;
			
		}
		
		PhysAddr = XSDT :: FindTable ( kSearchString );
		
	}
	
	if ( PhysAddr == NULL )
	{
		
		mm_kfree ( ProcessorLAPICRecords );
		mm_kfree ( IOAPICRecords );
		mm_kfree ( InterruptSourceOverrideRecords );
		
		return;
		
	}
	
	#ifdef KSTARTUP_DEBUG
	system_func_kprintf ( "MADT physical found!\n" );
	#endif
	
	Table = reinterpret_cast <MADTable *> ( mm_kvmap ( PhysAddr, 0x2000, MM::Paging::PageTable :: Flags_Writeable ) );
	
	if ( Table == NULL )
	{
		
		mm_kfree ( ProcessorLAPICRecords );
		mm_kfree ( IOAPICRecords );
		mm_kfree ( InterruptSourceOverrideRecords );
		
		return;
		
	}
	
	TableLength = ( reinterpret_cast <uint32_t> ( PhysAddr ) + Table -> Header.Length ) - ( reinterpret_cast <uint32_t> ( PhysAddr ) & 0xFFFFF000 );
	
	if ( TableLength > 0x2000 )
	{
		
		mm_kvunmap ( Table );
		Table = reinterpret_cast <MADTable *> ( mm_kvmap ( PhysAddr, TableLength, MM::Paging::PageTable :: Flags_Writeable ) );
		
		if ( Table == NULL )
		{
			
			mm_kfree ( ProcessorLAPICRecords );
			mm_kfree ( IOAPICRecords );
			mm_kfree ( InterruptSourceOverrideRecords );
			
			return;
			
		}
		
	}
	
	if ( ! ACPITable :: VerifyTable ( & Table -> Header ) )
	{
		
		mm_kvunmap ( Table );
		
		mm_kfree ( ProcessorLAPICRecords );
		mm_kfree ( IOAPICRecords );
		mm_kfree ( InterruptSourceOverrideRecords );
		
		return;
		
	}
	
	#ifdef KSTARTUP_DEBUG
	system_func_kprintf ( "MADT checksum ok!\n" );
	#endif
	
	RecordBase = reinterpret_cast <RecordHeader *> ( reinterpret_cast <uint32_t> ( Table ) + sizeof ( MADTable ) );
	
	#ifdef KSTARTUP_DEBUG
	system_func_kprintf ( "MADT report:\nLocal APIC Address: %h\n", Table -> LAPICAddress );
	#endif
	
	ProcessorLAPICRecord * PLRRecord;
	IOAPICRecord * IOARecord;
	InterruptSourceOverride * ISORecord;
	
	while ( reinterpret_cast <uint32_t> ( RecordBase ) < reinterpret_cast <uint32_t> ( Table ) + Table -> Header.Length )
	{
		
		switch ( RecordBase -> Type )
		{
			
			case kRecordType_ProcessorLAPICRecord:
			
			PLRRecord = reinterpret_cast <ProcessorLAPICRecord *> ( RecordBase );
			
			#ifdef KSTARTUP_DEBUG
			system_func_kprintf ( "* Processor LAPIC: [ APIC processor ID: %u, APIC ID: %u, Flags: %h ]\n", PLRRecord -> APICProcessorID, PLRRecord -> APICID, PLRRecord -> Flags );
			#endif
			
			ProcessorLAPICRecords -> Push ( PLRRecord );
			
			break;
			
			case kRecordType_IOAPICRecord:
			
			IOARecord = reinterpret_cast <IOAPICRecord *> ( RecordBase );
			
			#ifdef KSTARTUP_DEBUG
			system_func_kprintf ( "* I/O Apic: [ ID: %u, Address: %h, Global system interrupt base: %h ]\n", IOARecord -> ID, IOARecord -> Address, IOARecord -> GlobalSystemInterruptBase );
			#endif
			
			IOAPICRecords -> Push ( IOARecord );
			
			break;
			
			case kRecordType_InterruptSourceOverride:
			
			ISORecord = reinterpret_cast <InterruptSourceOverride *> ( RecordBase );
			
			#ifdef KSTARTUP_DEBUG
			system_func_kprintf ( "* Int src override: [ Bus: %u, IRQ: %u, Global system int: %u, Flags: %u ]\n", ISORecord -> BusSource, ISORecord -> IRQSource, ISORecord -> GlobalSystemInterrupt, ISORecord -> Flags );
			#endif
			
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
	
	Table = NULL;
	
};
