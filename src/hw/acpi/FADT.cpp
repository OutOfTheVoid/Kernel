#include <hw/acpi/FADT.h>
#include <hw/acpi/RSDT.h>
#include <hw/acpi/XSDT.h>
#include <hw/acpi/RSDP.h>

#include <mm/KVMap.h>
#include <mm/paging/PageTable.h>

#include <hw/cpu/IO.h>

#include <stddef.h>

bool HW::ACPI::FADT :: Validated = false;

const char * HW::ACPI::FADT :: kSearchString = "FACP";

HW::ACPI::FADT :: FADTable * HW::ACPI::FADT :: Table = NULL;

MT::Synchronization::Spinlock :: Spinlock_t HW::ACPI::FADT :: Lock = MT::Synchronization::Spinlock :: Initializer ();

void HW::ACPI::FADT :: Init ( uint32_t * Status )
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	void * PhysAddr = NULL;
	uint32_t TableLength;
	
	if ( Validated )
	{
		
		MT::Synchronization::Spinlock :: Release ( & Lock );
		
		* Status = kACPIStatus_Success;
		
		return;
		
	}
	
	if ( ! RSDP :: Found () )
	{
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return;
		
	}
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
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
	
	Table = reinterpret_cast <FADTable *> ( mm_kvmap ( PhysAddr, 0x2000, MM::Paging::PageTable :: Flags_Writeable | MM::Paging::PageTable :: Flags_NoCache ) );
	
	if ( Table == NULL )
		return;
	
	TableLength = ( reinterpret_cast <uint32_t> ( PhysAddr ) + Table -> Header.Length ) - ( reinterpret_cast <uint32_t> ( PhysAddr ) & 0xFFFFF000 );
	
	if ( TableLength > 0x2000 )
	{
		
		mm_kvunmap ( Table );
		Table = reinterpret_cast <FADTable *> ( mm_kvmap ( PhysAddr, TableLength, MM::Paging::PageTable :: Flags_Writeable ) );
		
		if ( Table == NULL )
			return;
		
	}
	
	if ( ! ACPITable :: VerifyTable ( & Table -> Header ) )
	{
		
		mm_kvunmap ( Table );
		
		return;
		
	}
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	Validated = true;
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
	* Status = kACPIStatus_Success;
	
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
	Table = NULL;
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
};

uint8_t HW::ACPI::FADT :: GetPreferredPMProfile ( uint32_t * Status )
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	if ( ! Validated )
	{
		
		MT::Synchronization::Spinlock :: Release ( & Lock );
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return 0;
		
	}
	
	uint8_t Profile = Table -> PreferredPMProfile;
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
	* Status = kACPIStatus_Success;
	
	return Profile;
	
};

uint16_t HW::ACPI::FADT :: GetACPISystemControlInterrupt ( uint32_t * Status )
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	if ( ! Validated )
	{
		
		MT::Synchronization::Spinlock :: Release ( & Lock );
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return 0;
		
	}
	
	uint16_t Interrupt = Table -> SystemControlInterrupt;
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
	* Status = kACPIStatus_Success;
	
	return Interrupt;
	
};

uint32_t HW::ACPI::FADT :: GetSMICommandPort ( uint32_t * Status )
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	if ( ! Validated )
	{
		
		MT::Synchronization::Spinlock :: Release ( & Lock );
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return 0;
		
	}
	
	uint32_t Port = Table -> SMICommandPort;
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
	* Status = kACPIStatus_Success;
	
	return Port;
	
};

void HW::ACPI::FADT :: WriteACPIEnable ( bool Enable, uint32_t * Status )
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	if ( ! Validated )
	{
		
		MT::Synchronization::Spinlock :: Release ( & Lock );
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return;
		
	}
	
	uint8_t Command;
	uint32_t Port = Table -> SMICommandPort;
	
	if ( Enable )
		Command = Table -> ACPIEnableCommand;
	else
		Command = Table -> ACPIDisableCommand;
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
	if ( Port == 0 ) // SMI not supported.
		return;
	
	HW::CPU::IO :: Out8 ( Port, Command );
	
	* Status = kACPIStatus_Success;
	
};

HW::ACPI :: ACPIAddress HW::ACPI::FADT :: GetPM1aEventBlockAddress ( uint32_t * Status )
{
	
	ACPIAddress Address;
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	if ( ! Validated )
	{
		
		MT::Synchronization::Spinlock :: Release ( & Lock );
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return Address;
		
	}
	
	Address.Address = Table -> PM1aEventBlockAddress;
	
	if ( Address.Address == 0 )
		Address = Table -> ExtendedPM1aEventBlockAddress;
	else
	{
		
		Address.AddressSpaceID = kACPIAddress_AddressSpaceID_Memory;
		Address.RegisterBitWidth = 0;
		Address.RegisterBitOffset = 0;
		
	}
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
	* Status = kACPIStatus_Success;
	
	return Address;
	
};

HW::ACPI :: ACPIAddress HW::ACPI::FADT :: GetPM1bEventBlockAddress ( uint32_t * Status )
{
	
	ACPIAddress Address;
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	if ( ! Validated )
	{
		
		MT::Synchronization::Spinlock :: Release ( & Lock );
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return Address;
		
	}
	
	Address.Address = Table -> PM1bEventBlockAddress;
	
	if ( Address.Address == 0 )
		Address = Table -> ExtendedPM1bEventBlockAddress;
	else
	{
		
		Address.AddressSpaceID = kACPIAddress_AddressSpaceID_Memory;
		Address.RegisterBitWidth = 0;
		Address.RegisterBitOffset = 0;
		
	}
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
	* Status = kACPIStatus_Success;
	
	return Address;
	
};

uint8_t HW::ACPI::FADT :: GetPM1EventBlockByteCount ( uint32_t * Status )
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	if ( ! Validated )
	{
		
		MT::Synchronization::Spinlock :: Release ( & Lock );
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return 0;
		
	}
	
	uint8_t Count = Table -> PM1EventBlockLength;
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
	* Status = kACPIStatus_Success;
	
	return Count;
	
};

HW::ACPI :: ACPIAddress HW::ACPI::FADT :: GetPM1aControlBlockAddress ( uint32_t * Status )
{
	
	ACPIAddress Address;
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	if ( ! Validated )
	{
		
		MT::Synchronization::Spinlock :: Release ( & Lock );
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return Address;
		
	}
	
	Address.Address = Table -> PM1aControlBlockAddress;
	
	if ( Address.Address == 0 )
		Address = Table -> ExtendedPM1aControlBlockAddress;
	else
	{
		
		Address.AddressSpaceID = kACPIAddress_AddressSpaceID_Memory;
		Address.RegisterBitWidth = 0;
		Address.RegisterBitOffset = 0;
		
	}
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
	* Status = kACPIStatus_Success;
	
	return Address;
	
};

HW::ACPI :: ACPIAddress HW::ACPI::FADT :: GetPM1bControlBlockAddress ( uint32_t * Status )
{
	
	ACPIAddress Address;
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	if ( ! Validated )
	{
		
		MT::Synchronization::Spinlock :: Release ( & Lock );
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return Address;
		
	}
	
	Address.Address = Table -> PM1bControlBlockAddress;
	
	if ( Address.Address == 0 )
		Address = Table -> ExtendedPM1bControlBlockAddress;
	else
	{
		
		Address.AddressSpaceID = kACPIAddress_AddressSpaceID_Memory;
		Address.RegisterBitWidth = 0;
		Address.RegisterBitOffset = 0;
		
	}
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
	* Status = kACPIStatus_Success;
	
	return Address;
	
};

uint8_t HW::ACPI::FADT :: GetPM1ControlBlockByteCount ( uint32_t * Status )
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	if ( ! Validated )
	{
		
		MT::Synchronization::Spinlock :: Release ( & Lock );
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return 0;
		
	}
	
	uint8_t Count = Table -> PM1ControlBlockLength;
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
	* Status = kACPIStatus_Success;
	
	return Count;
	
};

HW::ACPI :: ACPIAddress HW::ACPI::FADT :: GetPM2ControlBlockAddress ( uint32_t * Status )
{
	
	ACPIAddress Address;
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	if ( ! Validated )
	{
		
		MT::Synchronization::Spinlock :: Release ( & Lock );
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return Address;
		
	}
	
	Address.Address = Table -> PM2ControlBlockAddress;
	
	if ( Address.Address == 0 )
		Address = Table -> ExtendedPM2ControlBlockAddress;
	else
	{
		
		Address.AddressSpaceID = kACPIAddress_AddressSpaceID_Memory;
		Address.RegisterBitWidth = 0;
		Address.RegisterBitOffset = 0;
		
	}
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
	* Status = kACPIStatus_Success;
	
	return Address;
	
};

uint8_t HW::ACPI::FADT :: GetPM2ControlBlockByteCount ( uint32_t * Status )
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	if ( ! Validated )
	{
		
		MT::Synchronization::Spinlock :: Release ( & Lock );
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return 0;
		
	}
	
	uint8_t Count = Table -> PM2ControlBlockLength;
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
	* Status = kACPIStatus_Success;
	
	return Count;
	
};

HW::ACPI :: ACPIAddress HW::ACPI::FADT :: GetPMTimerBlockAddress ( uint32_t * Status )
{
	
	ACPIAddress Address;
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	if ( ! Validated )
	{
		
		MT::Synchronization::Spinlock :: Release ( & Lock );
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return Address;
		
	}
	
	Address.Address = Table -> PMTimerBlockAddress;
	
	if ( Address.Address == 0 )
		Address = Table -> ExtendedPMTimerBlockAddress;
	else
	{
		
		Address.AddressSpaceID = kACPIAddress_AddressSpaceID_Memory;
		Address.RegisterBitWidth = 0;
		Address.RegisterBitOffset = 0;
		
	}
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
	* Status = kACPIStatus_Success;
	
	return Address;
	
};

uint8_t HW::ACPI::FADT :: GetPMTimerBlockByteCount ( uint32_t * Status )
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	if ( ! Validated )
	{
		
		MT::Synchronization::Spinlock :: Release ( & Lock );
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return 0;
		
	}
	
	uint8_t Count = Table -> PMTimerBlockLength;
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
	* Status = kACPIStatus_Success;
	
	return Count;
	
};

HW::ACPI :: ACPIAddress HW::ACPI::FADT :: GetGeneralPurposeEvent0BlockAddress ( uint32_t * Status )
{
	
	ACPIAddress Address;
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	if ( ! Validated )
	{
		
		MT::Synchronization::Spinlock :: Release ( & Lock );
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return Address;
		
	}
	
	Address.Address = Table -> GPE0BlockAddress;
	
	if ( Address.Address == 0 )
		Address = Table -> ExtendedGPE0BlockAddress;
	else
	{
		
		Address.AddressSpaceID = kACPIAddress_AddressSpaceID_Memory;
		Address.RegisterBitWidth = 0;
		Address.RegisterBitOffset = 0;
		
	}
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
	* Status = kACPIStatus_Success;
	
	return Address;
	
};

HW::ACPI :: ACPIAddress HW::ACPI::FADT :: GetGeneralPurposeEvent1BlockAddress ( uint32_t * Status )
{
	
	ACPIAddress Address;
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	if ( ! Validated )
	{
		
		MT::Synchronization::Spinlock :: Release ( & Lock );
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return Address;
		
	}
	
	Address.Address = Table -> GPE1BlockAddress;
	
	if ( Address.Address == 0 )
		Address = Table -> ExtendedGPE1BlockAddress;
	else
	{
		
		Address.AddressSpaceID = kACPIAddress_AddressSpaceID_Memory;
		Address.RegisterBitWidth = 0;
		Address.RegisterBitOffset = 0;
		
	}
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
	* Status = kACPIStatus_Success;
	
	return Address;
	
};

uint8_t HW::ACPI::FADT :: GetGeneralPurposeEvent1Base ( uint32_t * Status )
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	if ( ! Validated )
	{
		
		MT::Synchronization::Spinlock :: Release ( & Lock );
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return 0;
		
	}
	
	uint8_t Base = Table -> GPE1Base;
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
	* Status = kACPIStatus_Success;
	
	return Base;
	
};

HW::ACPI :: ACPIAddress HW::ACPI::FADT :: GetResetRegisterAddress ( uint32_t * Status )
{
	
	ACPIAddress Address;
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	if ( ! Validated )
	{
		
		MT::Synchronization::Spinlock :: Release ( & Lock );
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return Address;
		
	}
	
	Address = Table -> ResetRegister;
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
	* Status = kACPIStatus_Success;
	
	return Address;
	
};

uint8_t HW::ACPI::FADT :: GetResetValue ( uint32_t * Status )
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	if ( ! Validated )
	{
		
		MT::Synchronization::Spinlock :: Release ( & Lock );
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return 0;
		
	}
	
	uint8_t Value = Table -> ResetValue;
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
	* Status = kACPIStatus_Success;
	
	return Value;
	
};

uint16_t HW::ACPI::FADT :: GetIAPCFlags ( uint32_t * Status )
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	if ( ! Validated )
	{
		
		MT::Synchronization::Spinlock :: Release ( & Lock );
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return 0;
		
	}
	
	uint32_t Value = Table -> IAPCFlags;
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
	* Status = kACPIStatus_Success;
	
	return Value;
	
};
