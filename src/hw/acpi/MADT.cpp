#include <hw/acpi/MADT.h>
#include <hw/acpi/RSDT.h>
#include <hw/acpi/XSDT.h>
#include <hw/acpi/RSDP.h>

#include <system/func/KPrintf.h>

#include <mm/KVMap.h>
#include <mm/paging/PageTable.h>

#include <KernelDef.h>

bool HW::ACPI::MADT :: Validated = false;
HW::ACPI::MADT :: MADTable * HW::ACPI::MADT :: Table = NULL;

const char * HW::ACPI::MADT :: kSearchString = "APIC";

void HW::ACPI::MADT :: Init ()
{
	
	RecordHeader * RecordBase;
	uint32_t RecordLength;
	
	if ( Validated )
		return;
	
	void * PhysAddr;
	
	if ( RSDP :: GetACPIRevision () == RSDP :: kACPI_Revision_1 )
	{
		
		if ( ! RSDT :: Valid () )
			return;
		
		PhysAddr = RSDT :: FindTable ( kSearchString );
		
	}
	else
	{
		
		if ( ! XSDT :: Valid () )
			return;
		
		PhysAddr = XSDT :: FindTable ( kSearchString );
		
	}
	
	if ( PhysAddr == NULL )
		return;
	
	#ifdef KSTARTUP_DEBUG
		system_func_kprintf ( "MADT physical found!\n" );
	#endif
	
	Table = reinterpret_cast <MADTable *> ( mm_kvmap ( PhysAddr, 0x2000, MM::Paging::PageTable :: Flags_Writeable ) );
	
	if ( Table == NULL )
		return;
	
	uint32_t TableLength = ( reinterpret_cast <uint32_t> ( PhysAddr ) + Table -> Header.Length ) - ( reinterpret_cast <uint32_t> ( PhysAddr ) & 0xFFFFF000 );
	
	if ( TableLength > 0x2000 )
	{
		
		mm_kvunmap ( Table );
		Table = reinterpret_cast <MADTable *> ( mm_kvmap ( PhysAddr, TableLength, MM::Paging::PageTable :: Flags_Writeable ) );
		
		if ( Table == NULL )
			return;
		
	}
	
	if ( ! ACPITable :: VerifyTable ( & Table -> Header ) )
	{
		
		mm_kvunmap ( Table );
		
		return;
		
	}
	
	#ifdef KSTARTUP_DEBUG
		system_func_kprintf ( "MADT checksum ok!\n" );
	#endif
	
	RecordLength = 28;
	RecordBase = reinterpret_cast <RecordHeader *> ( reinterpret_cast <uint32_t> ( Table ) + 28 );
	
	#ifdef KSTARTUP_DEBUG
		system_func_kprintf ( "MADT length:%u\nMADT report:\n", Table -> Header.Length );
	#endif
	
	ProcessorLAPICRecord * PLRRecord;
	IOAPICRecord * IOARecord;
	InterruptSourceOverride * ISORecord;
	
	while ( RecordLength < Table -> Header.Length )
	{
		
		switch ( RecordBase -> Type )
		{
			
		case kRecordType_ProcessorLAPICRecord:
		
			PLRRecord = reinterpret_cast <ProcessorLAPICRecord *> ( RecordBase );
			
			#ifdef KSTARTUP_DEBUG
				system_func_kprintf ( "* Processor LAPIC: [ APIC processor ID: %u, APIC ID: %u, Flags: %h ]\n", PLRRecord -> APICProcessorID, PLRRecord -> APICID, PLRRecord -> Flags );
			#endif
			
			break;
			
		case kRecordType_IOAPICRecord:
			
			IOARecord = reinterpret_cast <IOAPICRecord *> ( RecordBase );
			
			#ifdef KSTARTUP_DEBUG
				system_func_kprintf ( "* I/O Apic: [ ID: %u, Address: %h, Global system interrupt base: %h ]\n", IOARecord -> ID, IOARecord -> Address, IOARecord -> GlobalSystemInterruptBase );
			#endif
			
			break;
			
		case kRecordType_InterruptSourceOverride:
		
			ISORecord = reinterpret_cast <InterruptSourceOverride *> ( RecordBase );
		
			#ifdef KSTARTUP_DEBUG
				system_func_kprintf ( "* Interrupt source oberride: [ Bus source: %u, IRQ source: %u, Global system interrupt: %u, Flags: %u ]\n", ISORecord -> BusSource, ISORecord -> IRQSource, ISORecord -> GlobalSystemInterrupt, ISORecord -> Flags );
			#endif
		
			break;
			
		}
		
		RecordLength += RecordBase -> Length;
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
