#include <hw/acpi/FADT.h>
#include <hw/acpi/RSDT.h>
#include <hw/acpi/XSDT.h>
#include <hw/acpi/RSDP.h>

bool HW::ACPI::FADT :: Validated = false;

const char * HW::ACPI::FADT :: kSearchString = "FACP";

HW::ACPI::FADT :: FADTable * HW::ACPI::FADT :: Table = NULL;

MT::Synchronization::Spinlock :: Spinlock_t HW::ACPI::MADT :: Lock = MT::Synchronization::Spinlock :: Initializer ();

void HW::ACPI::FADT :: Init ()
{
	
	void * PhysAddr = NULL;
	
	if ( Validated || ! RSDP :: Found () )
		return;
	
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
	
	Table = reinterpret_cast <MADTable *> ( mm_kvmap ( PhysAddr, 0x2000, MM::Paging::PageTable :: Flags_Writeable | MM::Paging::PageTable :: Flags_NoCache ) );
	
	if ( Table == NULL )
		return;
	
	TableLength = ( reinterpret_cast <uint32_t> ( PhysAddr ) + Table -> Header.Length ) - ( reinterpret_cast <uint32_t> ( PhysAddr ) & 0xFFFFF000 );
	
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
	
};

bool HW::ACPI::FADT :: Valid ()
{
	
	return Validated;
	
};

void HW::ACPI::FADT :: Discard ()
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	Validated = false;
	
	mm_kvunmap ( Table );
	Table = NULL:
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
};

uint8_t HW::ACPI::FADT :: GetPreferredPMProfile ()
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	if ( ! Validated )
		KPANIC ( "Attempt to get FADT value, but it wasn't a valid ACPI table!" );
	
	return ;
	
};

uint16_t HW::ACPI::FADT :: GetACPISystemControlInterrupt ()
{
	
	if ( ! Validated )
		KPANIC ( "Attempt to get FADT value, but it wasn't a valid ACPI table!" );
	
	return 0;
	
};

uint32_t HW::ACPI::FADT :: GetSMICommandPort ()
{
	
	if ( ! Validated )
		KPANIC ( "Attempt to get FADT value, but it wasn't a valid ACPI table!" );
	
	return 0;
	
};

void HW::ACPI::FADT :: WriteACPIEnable ( bool Enable )
{
	
	if ( ! Validated )
		KPANIC ( "Attempt write ACPI Enable, but the FADT wasn't a valid ACPI table!" );
	
};

uint64_t HW::ACPI::FADT :: GetPM1aEventBlockAddress ()
{
	
	if ( ! Validated )
		KPANIC ( "Attempt to get FADT value, but it wasn't a valid ACPI table!" );
	
	return 0;
	
};

uint64_t HW::ACPI::FADT :: GetPM1bEventBlockAddress ()
{
	
	if ( ! Validated )
		KPANIC ( "Attempt to get FADT value, but it wasn't a valid ACPI table!" );
	
	return 0;
	
};

uint64_t HW::ACPI::FADT :: GetPM1EventBlockByteCount ()
{
	
	if ( ! Validated )
		KPANIC ( "Attempt to get FADT value, but it wasn't a valid ACPI table!" );
	
	return 0;
	
};

uint64_t HW::ACPI::FADT :: GetPM1aControlBlockAddress ()
{
	
	if ( ! Validated )
		KPANIC ( "Attempt to get FADT value, but it wasn't a valid ACPI table!" );
	
	return 0;
	
};

uint64_t HW::ACPI::FADT :: GetPM1bControlBlockAddress ()
{
	
	if ( ! Validated )
		KPANIC ( "Attempt to get FADT value, but it wasn't a valid ACPI table!" );
	
	return 0;
	
};

uint64_t HW::ACPI::FADT :: GetPM1ControlBlockByteCount ()
{
	
	if ( ! Validated )
		KPANIC ( "Attempt to get FADT value, but it wasn't a valid ACPI table!" );
	
	return 0;
	
};

uint64_t HW::ACPI::FADT :: GetPM2ControlBlockAddress ()
{
	
	if ( ! Validated )
		KPANIC ( "Attempt to get FADT value, but it wasn't a valid ACPI table!" );
	
	return 0;
	
};

uint64_t HW::ACPI::FADT :: GetPM2ControlBlockByteCount ()
{
	
	if ( ! Validated )
		KPANIC ( "Attempt to get FADT value, but it wasn't a valid ACPI table!" );
	
	return 0;
	
};

uint64_t HW::ACPI::FADT :: GetPMTimerBlockAddress ()
{
	
	if ( ! Validated )
		KPANIC ( "Attempt to get FADT value, but it wasn't a valid ACPI table!" );
	
	return 0;
	
};

uint64_t HW::ACPI::FADT :: GetPMTimerBlockByteCount ()
{
	
	if ( ! Validated )
		KPANIC ( "Attempt to get FADT value, but it wasn't a valid ACPI table!" );
	
	return 0;
	
};

uint64_t HW::ACPI::FADT :: GetGeneralPurposeEvent0BlockAddress ()
{
	
	if ( ! Validated )
		KPANIC ( "Attempt to get FADT value, but it wasn't a valid ACPI table!" );
	
	return 0;
	
};

uint64_t HW::ACPI::FADT :: GetGeneralPurposeEvent1BlockAddress ()
{
	
	if ( ! Validated )
		KPANIC ( "Attempt to get FADT value, but it wasn't a valid ACPI table!" );
	
	return 0;
	
};

uint64_t HW::ACPI::FADT :: GetGeneralPurposeEvent1Base ()
{
	
	if ( ! Validated )
		KPANIC ( "Attempt to get FADT value, but it wasn't a valid ACPI table!" );
	
	return 0;
	
};

HW::ACPI::ACPITable :: ACPIAddress HW::ACPI::FADT :: GetResetRegisterAddress ()
{
	
	ACPITable :: ACPIAddress Address;
	
	return Address;
	
};

uint8_t HW::ACPI::FADT :: GetResetValue ()
{
	
	if ( ! Validated )
		KPANIC ( "Attempt to get FADT value, but it wasn't a valid ACPI table!" );
	
	return 0;
	
};
