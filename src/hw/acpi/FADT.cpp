#include <hw/acpi/FADT.h>
#include <hw/acpi/RSDT.h>
#include <hw/acpi/XSDT.h>
#include <hw/acpi/RSDP.h>

#include <mm/KVMap.h>
#include <mm/paging/PageTable.h>

#include <util/Vector.h>

#include <hw/cpu/IO.h>

#include <stddef.h>

bool HW::ACPI::FADT :: Validated = false;

const char * HW::ACPI::FADT :: kSearchString = "FACP";

HW::ACPI::FADT :: FADTable * HW::ACPI::FADT :: Table = NULL;

void HW::ACPI::FADT :: Init ( uint32_t * Status )
{
	
	void * PhysAddr = NULL;
	uint32_t TableLength;
	
	if ( Validated )
	{
		
		* Status = kACPIStatus_Success;
		
		return;
		
	}
	
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
		
		PhysAddr = RSDT :: FindTable ( kSearchString );
			
	}
	else
	{
		
		if ( ! XSDT :: Valid () )
		{
			
			* Status = kACPIStatus_Failure_InvalidTable;
			
			return;
			
		}
		
		PhysAddr = XSDT :: FindTable ( kSearchString );
		
	}
	
	if ( PhysAddr == NULL )
	{
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return;
		
	}
	
	Table = reinterpret_cast <FADTable *> ( mm_kvmap ( PhysAddr, 0x2000, MM::Paging::PageTable :: Flags_Writeable | MM::Paging::PageTable :: Flags_NoCache ) );
	
	if ( Table == NULL )
	{
		
		* Status = kACPIStatus_Failure_System_OutOfMemory;
		
		return;
		
	}
	
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
	
	Vector <uint32_t> CurrentKVMapPages;
	
	if ( Table -> PM1aEventBlockAddress != 0 )
	{
		
		void * MappedRegister = NULL;
		
		MappedRegister = mm_kvmap ( reinterpret_cast <void *> ( Table -> PM1aEventBlockAddress ), 0x1000, MM::Paging::PageTable :: Flags_NoCache );
		
		if ( MappedRegister == NULL )
		{
				
			mm_kvunmap ( Table );
		
			* Status = kACPIStatus_Failure_System_OutOfMemory;
		
			return;
			
		}
		
		CurrentKVMapPages.Push ( reinterpret_cast <uint32_t> ( MappedRegister ) & 0xFFFFF000 );
		
		Table -> PM1aEventBlockAddress = reinterpret_cast <uint32_t> ( MappedRegister );
		
	}
	
	if ( Table -> PM1bEventBlockAddress != 0 )
	{
		
		void * MappedRegister = NULL;
		
		for ( uint32_t I = 0; I < CurrentKVMapPages.Length (); I ++ )
		{
			
			uint32_t PAdd = MM::Paging::PageTable :: KernelVirtualToPhysical ( CurrentKVMapPages [ I ] );
			
			if ( ( PAdd <= Table -> PM1bEventBlockAddress ) && ( ( PAdd + 0x1000 ) > Table -> PM1bEventBlockAddress ) )
				MappedRegister = reinterpret_cast <void *> ( CurrentKVMapPages [ I ] + ( Table -> PM1bEventBlockAddress & 0x00000FFF ) );
			
		}
		
		if ( MappedRegister == NULL )
		{
			
			MappedRegister = mm_kvmap ( reinterpret_cast <void *> ( Table -> PM1bEventBlockAddress ), 0x1000, MM::Paging::PageTable :: Flags_NoCache );
			
			if ( MappedRegister == NULL )
			{
				
				while ( CurrentKVMapPages.Length () != 0 )
					mm_kvunmap ( reinterpret_cast <void *> ( CurrentKVMapPages.Pop () ) );
					
				mm_kvunmap ( Table );
				
				* Status = kACPIStatus_Failure_System_OutOfMemory;
			
				return;
				
			}
			
			CurrentKVMapPages.Push ( reinterpret_cast <uint32_t> ( MappedRegister ) & 0xFFFFF000 );
			
		}
		
		Table -> PM1bEventBlockAddress = reinterpret_cast <uint32_t> ( MappedRegister );
		
	}
	
	if ( Table -> PM1aControlBlockAddress != 0 )
	{
		
		void * MappedRegister = NULL;
		
		for ( uint32_t I = 0; I < CurrentKVMapPages.Length (); I ++ )
		{
			
			uint32_t PAdd = MM::Paging::PageTable :: KernelVirtualToPhysical ( CurrentKVMapPages [ I ] );
			
			if ( ( PAdd <= Table -> PM1aControlBlockAddress ) && ( ( PAdd + 0x1000 ) > Table -> PM1aControlBlockAddress ) )
				MappedRegister = reinterpret_cast <void *> ( CurrentKVMapPages [ I ] + ( Table -> PM1aControlBlockAddress & 0x00000FFF ) );
			
		}
		
		if ( MappedRegister == NULL )
		{
			
			MappedRegister = mm_kvmap ( reinterpret_cast <void *> ( Table -> PM1aControlBlockAddress ), 0x1000, MM::Paging::PageTable :: Flags_NoCache );
			
			if ( MappedRegister == NULL )
			{
				
				while ( CurrentKVMapPages.Length () != 0 )
					mm_kvunmap ( reinterpret_cast <void *> ( CurrentKVMapPages.Pop () ) );
					
				mm_kvunmap ( Table );
				
				* Status = kACPIStatus_Failure_System_OutOfMemory;
			
				return;
				
			}
			
			CurrentKVMapPages.Push ( reinterpret_cast <uint32_t> ( MappedRegister ) & 0xFFFFF000 );
			
		}
		
		Table -> PM1aControlBlockAddress = reinterpret_cast <uint32_t> ( MappedRegister );
		
	}
	
	if ( Table -> PM1bControlBlockAddress != 0 )
	{
		
		void * MappedRegister = NULL;
		
		for ( uint32_t I = 0; I < CurrentKVMapPages.Length (); I ++ )
		{
			
			uint32_t PAdd = MM::Paging::PageTable :: KernelVirtualToPhysical ( CurrentKVMapPages [ I ] );
			
			if ( ( PAdd <= Table -> PM1bControlBlockAddress ) && ( ( PAdd + 0x1000 ) > Table -> PM1bControlBlockAddress ) )
				MappedRegister = reinterpret_cast <void *> ( CurrentKVMapPages [ I ] + ( Table -> PM1bControlBlockAddress & 0x00000FFF ) );
			
		}
		
		if ( MappedRegister == NULL )
		{
			
			MappedRegister = mm_kvmap ( reinterpret_cast <void *> ( Table -> PM1bControlBlockAddress ), 0x1000, MM::Paging::PageTable :: Flags_NoCache );
			
			if ( MappedRegister == NULL )
			{
				
				while ( CurrentKVMapPages.Length () != 0 )
					mm_kvunmap ( reinterpret_cast <void *> ( CurrentKVMapPages.Pop () ) );
					
				mm_kvunmap ( Table );
				
				* Status = kACPIStatus_Failure_System_OutOfMemory;
			
				return;
				
			}
			
			CurrentKVMapPages.Push ( reinterpret_cast <uint32_t> ( MappedRegister ) & 0xFFFFF000 );
			
		}
		
		Table -> PM1bControlBlockAddress = reinterpret_cast <uint32_t> ( MappedRegister );
		
	}
	
	if ( Table -> PM2ControlBlockAddress != 0 )
	{
		
		void * MappedRegister = NULL;
		
		for ( uint32_t I = 0; I < CurrentKVMapPages.Length (); I ++ )
		{
			
			uint32_t PAdd = MM::Paging::PageTable :: KernelVirtualToPhysical ( CurrentKVMapPages [ I ] );
			
			if ( ( PAdd <= Table -> PM2ControlBlockAddress ) && ( ( PAdd + 0x1000 ) > Table -> PM2ControlBlockAddress ) )
				MappedRegister = reinterpret_cast <void *> ( CurrentKVMapPages [ I ] + ( Table -> PM2ControlBlockAddress & 0x00000FFF ) );
			
		}
		
		if ( MappedRegister == NULL )
		{
			
			MappedRegister = mm_kvmap ( reinterpret_cast <void *> ( Table -> PM2ControlBlockAddress ), 0x1000, MM::Paging::PageTable :: Flags_NoCache );
			
			if ( MappedRegister == NULL )
			{
				
				while ( CurrentKVMapPages.Length () != 0 )
					mm_kvunmap ( reinterpret_cast <void *> ( CurrentKVMapPages.Pop () ) );
					
				mm_kvunmap ( Table );
				
				* Status = kACPIStatus_Failure_System_OutOfMemory;
			
				return;
				
			}
			
			CurrentKVMapPages.Push ( reinterpret_cast <uint32_t> ( MappedRegister ) & 0xFFFFF000 );
			
		}
		
		Table -> PM2ControlBlockAddress = reinterpret_cast <uint32_t> ( MappedRegister );
		
	}
	
	if ( Table -> PMTimerBlockAddress != 0 )
	{
		
		void * MappedRegister = NULL;
		
		for ( uint32_t I = 0; I < CurrentKVMapPages.Length (); I ++ )
		{
			
			uint32_t PAdd = MM::Paging::PageTable :: KernelVirtualToPhysical ( CurrentKVMapPages [ I ] );
			
			if ( ( PAdd <= Table -> PMTimerBlockAddress ) && ( ( PAdd + 0x1000 ) > Table -> PMTimerBlockAddress ) )
				MappedRegister = reinterpret_cast <void *> ( CurrentKVMapPages [ I ] + ( Table -> PMTimerBlockAddress & 0x00000FFF ) );
			
		}
		
		if ( MappedRegister == NULL )
		{
			
			MappedRegister = mm_kvmap ( reinterpret_cast <void *> ( Table -> PMTimerBlockAddress ), 0x1000, MM::Paging::PageTable :: Flags_NoCache );
			
			if ( MappedRegister == NULL )
			{
				
				while ( CurrentKVMapPages.Length () != 0 )
					mm_kvunmap ( reinterpret_cast <void *> ( CurrentKVMapPages.Pop () ) );
					
				mm_kvunmap ( Table );
				
				* Status = kACPIStatus_Failure_System_OutOfMemory;
			
				return;
				
			}
			
			CurrentKVMapPages.Push ( reinterpret_cast <uint32_t> ( MappedRegister ) & 0xFFFFF000 );
			
		}
		
		Table -> PMTimerBlockAddress = reinterpret_cast <uint32_t> ( MappedRegister );
		
	}
	
	if ( Table -> GPE0BlockAddress != 0 )
	{
		
		void * MappedRegister = NULL;
		
		for ( uint32_t I = 0; I < CurrentKVMapPages.Length (); I ++ )
		{
			
			uint32_t PAdd = MM::Paging::PageTable :: KernelVirtualToPhysical ( CurrentKVMapPages [ I ] );
			
			if ( ( PAdd <= Table -> GPE0BlockAddress ) && ( ( PAdd + 0x1000 ) > Table -> GPE0BlockAddress ) )
				MappedRegister = reinterpret_cast <void *> ( CurrentKVMapPages [ I ] + ( Table -> GPE0BlockAddress & 0x00000FFF ) );
			
		}
		
		if ( MappedRegister == NULL )
		{
			
			MappedRegister = mm_kvmap ( reinterpret_cast <void *> ( Table -> GPE0BlockAddress ), 0x1000, MM::Paging::PageTable :: Flags_NoCache );
			
			if ( MappedRegister == NULL )
			{
				
				while ( CurrentKVMapPages.Length () != 0 )
					mm_kvunmap ( reinterpret_cast <void *> ( CurrentKVMapPages.Pop () ) );
					
				mm_kvunmap ( Table );
				
				* Status = kACPIStatus_Failure_System_OutOfMemory;
			
				return;
				
			}
			
			CurrentKVMapPages.Push ( reinterpret_cast <uint32_t> ( MappedRegister ) & 0xFFFFF000 );
			
		}
		
		Table -> GPE0BlockAddress = reinterpret_cast <uint32_t> ( MappedRegister );
		
	}
	
	if ( Table -> GPE1BlockAddress != 0 )
	{
		
		void * MappedRegister = NULL;
		
		for ( uint32_t I = 0; I < CurrentKVMapPages.Length (); I ++ )
		{
			
			uint32_t PAdd = MM::Paging::PageTable :: KernelVirtualToPhysical ( CurrentKVMapPages [ I ] );
			
			if ( ( PAdd <= Table -> GPE1BlockAddress ) && ( ( PAdd + 0x1000 ) > Table -> GPE1BlockAddress ) )
				MappedRegister = reinterpret_cast <void *> ( CurrentKVMapPages [ I ] + ( Table -> GPE1BlockAddress & 0x00000FFF ) );
			
		}
		
		if ( MappedRegister == NULL )
		{
			
			MappedRegister = mm_kvmap ( reinterpret_cast <void *> ( Table -> GPE1BlockAddress ), 0x1000, MM::Paging::PageTable :: Flags_NoCache );
			
			if ( MappedRegister == NULL )
			{
				
				while ( CurrentKVMapPages.Length () != 0 )
					mm_kvunmap ( reinterpret_cast <void *> ( CurrentKVMapPages.Pop () ) );
					
				mm_kvunmap ( Table );
				
				* Status = kACPIStatus_Failure_System_OutOfMemory;
			
				return;
				
			}
			
			CurrentKVMapPages.Push ( reinterpret_cast <uint32_t> ( MappedRegister ) & 0xFFFFF000 );
			
		}
		
		Table -> GPE1BlockAddress = reinterpret_cast <uint32_t> ( MappedRegister );
		
	}
	
	// Reset Register
	
	if ( ( Table -> ResetRegister.Address != 0 ) && ( Table -> ResetRegister.AddressSpaceID == kACPIAddress_AddressSpaceID_Memory ) )
	{
		
		void * MappedRegister = NULL;
		
		for ( uint32_t I = 0; I < CurrentKVMapPages.Length (); I ++ )
		{
			
			uint32_t PAdd = MM::Paging::PageTable :: KernelVirtualToPhysical ( CurrentKVMapPages [ I ] );
			
			if ( ( PAdd <= Table -> ResetRegister.Address ) && ( ( PAdd + 0x1000 ) > Table -> ResetRegister.Address ) )
				MappedRegister = reinterpret_cast <void *> ( CurrentKVMapPages [ I ] + ( Table -> ResetRegister.Address & 0x00000FFF ) );
			
		}
		
		if ( MappedRegister == NULL )
		{
			
			MappedRegister = mm_kvmap ( reinterpret_cast <void *> ( Table -> ResetRegister.Address ), 0x1000, MM::Paging::PageTable :: Flags_NoCache );
			
			if ( MappedRegister == NULL )
			{
				
				while ( CurrentKVMapPages.Length () != 0 )
					mm_kvunmap ( reinterpret_cast <void *> ( CurrentKVMapPages.Pop () ) );
					
				mm_kvunmap ( Table );
				
				* Status = kACPIStatus_Failure_System_OutOfMemory;
			
				return;
				
			}
			
			CurrentKVMapPages.Push ( reinterpret_cast <uint32_t> ( MappedRegister ) & 0xFFFFF000 );
			
		}
		
		Table -> ResetRegister.Address = reinterpret_cast <uint64_t> ( MappedRegister );
		
	}
	
	// ExtendedPM1aEventBlockAddress
	
	if ( ( Table -> PM1aEventBlockAddress == 0 ) && ( Table -> ExtendedPM1aEventBlockAddress.Address != 0 ) && ( Table -> ExtendedPM1aEventBlockAddress.AddressSpaceID == kACPIAddress_AddressSpaceID_Memory ) )
	{
		
		void * MappedRegister = NULL;
		
		for ( uint32_t I = 0; I < CurrentKVMapPages.Length (); I ++ )
		{
			
			uint32_t PAdd = MM::Paging::PageTable :: KernelVirtualToPhysical ( CurrentKVMapPages [ I ] );
			
			if ( ( PAdd <= Table -> ExtendedPM1aEventBlockAddress.Address ) && ( ( PAdd + 0x1000 ) > Table -> ExtendedPM1aEventBlockAddress.Address ) )
				MappedRegister = reinterpret_cast <void *> ( CurrentKVMapPages [ I ] + ( Table -> ExtendedPM1aEventBlockAddress.Address & 0x00000FFF ) );
			
		}
		
		if ( MappedRegister == NULL )
		{
			
			MappedRegister = mm_kvmap ( reinterpret_cast <void *> ( Table -> ExtendedPM1aEventBlockAddress.Address ), 0x1000, MM::Paging::PageTable :: Flags_NoCache );
			
			if ( MappedRegister == NULL )
			{
				
				while ( CurrentKVMapPages.Length () != 0 )
					mm_kvunmap ( reinterpret_cast <void *> ( CurrentKVMapPages.Pop () ) );
					
				mm_kvunmap ( Table );
				
				* Status = kACPIStatus_Failure_System_OutOfMemory;
			
				return;
				
			}
			
			CurrentKVMapPages.Push ( reinterpret_cast <uint32_t> ( MappedRegister ) & 0xFFFFF000 );
			
		}
		
		Table -> ExtendedPM1aEventBlockAddress.Address = reinterpret_cast <uint64_t> ( MappedRegister );
		
	}
	
	// ExtendedPM1bEventBlockAddress
	
	if ( ( Table -> PM1bEventBlockAddress == 0 ) && ( Table -> ExtendedPM1bEventBlockAddress.Address != 0 ) && ( Table -> ExtendedPM1bEventBlockAddress.AddressSpaceID == kACPIAddress_AddressSpaceID_Memory ) )
	{
		
		void * MappedRegister = NULL;
		
		for ( uint32_t I = 0; I < CurrentKVMapPages.Length (); I ++ )
		{
			
			uint32_t PAdd = MM::Paging::PageTable :: KernelVirtualToPhysical ( CurrentKVMapPages [ I ] );
			
			if ( ( PAdd <= Table -> ExtendedPM1bEventBlockAddress.Address ) && ( ( PAdd + 0x1000 ) > Table -> ExtendedPM1bEventBlockAddress.Address ) )
				MappedRegister = reinterpret_cast <void *> ( CurrentKVMapPages [ I ] + ( Table -> ExtendedPM1bEventBlockAddress.Address & 0x00000FFF ) );
			
		}
		
		if ( MappedRegister == NULL )
		{
			
			MappedRegister = mm_kvmap ( reinterpret_cast <void *> ( Table -> ExtendedPM1bEventBlockAddress.Address ), 0x1000, MM::Paging::PageTable :: Flags_NoCache );
			
			if ( MappedRegister == NULL )
			{
				
				while ( CurrentKVMapPages.Length () != 0 )
					mm_kvunmap ( reinterpret_cast <void *> ( CurrentKVMapPages.Pop () ) );
					
				mm_kvunmap ( Table );
				
				* Status = kACPIStatus_Failure_System_OutOfMemory;
			
				return;
				
			}
			
			CurrentKVMapPages.Push ( reinterpret_cast <uint32_t> ( MappedRegister ) & 0xFFFFF000 );
			
		}
		
		Table -> ExtendedPM1bEventBlockAddress.Address = reinterpret_cast <uint64_t> ( MappedRegister );
		
	}
	
	// ExtendedPM1aControlBlockAddress
	
	if ( ( Table -> PM1aControlBlockAddress == 0 ) && ( Table -> ExtendedPM1aControlBlockAddress.Address != 0 ) && ( Table -> ExtendedPM1aControlBlockAddress.AddressSpaceID == kACPIAddress_AddressSpaceID_Memory ) )
	{
		
		void * MappedRegister = NULL;
		
		for ( uint32_t I = 0; I < CurrentKVMapPages.Length (); I ++ )
		{
			
			uint32_t PAdd = MM::Paging::PageTable :: KernelVirtualToPhysical ( CurrentKVMapPages [ I ] );
			
			if ( ( PAdd <= Table -> ExtendedPM1aControlBlockAddress.Address ) && ( ( PAdd + 0x1000 ) > Table -> ExtendedPM1aControlBlockAddress.Address ) )
				MappedRegister = reinterpret_cast <void *> ( CurrentKVMapPages [ I ] + ( Table -> ExtendedPM1aControlBlockAddress.Address & 0x00000FFF ) );
			
		}
		
		if ( MappedRegister == NULL )
		{
			
			MappedRegister = mm_kvmap ( reinterpret_cast <void *> ( Table -> ExtendedPM1aControlBlockAddress.Address ), 0x1000, MM::Paging::PageTable :: Flags_NoCache );
			
			if ( MappedRegister == NULL )
			{
				
				while ( CurrentKVMapPages.Length () != 0 )
					mm_kvunmap ( reinterpret_cast <void *> ( CurrentKVMapPages.Pop () ) );
					
				mm_kvunmap ( Table );
				
				* Status = kACPIStatus_Failure_System_OutOfMemory;
			
				return;
				
			}
			
			CurrentKVMapPages.Push ( reinterpret_cast <uint32_t> ( MappedRegister ) & 0xFFFFF000 );
			
		}
		
		Table -> ExtendedPM1aControlBlockAddress.Address = reinterpret_cast <uint64_t> ( MappedRegister );
		
	}
	
	// ExtendedPM1bControlBlockAddress
	
	if ( ( Table -> PM1bControlBlockAddress == 0 ) && ( Table -> ExtendedPM1bControlBlockAddress.Address != 0 ) && ( Table -> ExtendedPM1bControlBlockAddress.AddressSpaceID == kACPIAddress_AddressSpaceID_Memory ) )
	{
		
		void * MappedRegister = NULL;
		
		for ( uint32_t I = 0; I < CurrentKVMapPages.Length (); I ++ )
		{
			
			uint32_t PAdd = MM::Paging::PageTable :: KernelVirtualToPhysical ( CurrentKVMapPages [ I ] );
			
			if ( ( PAdd <= Table -> ExtendedPM1bControlBlockAddress.Address ) && ( ( PAdd + 0x1000 ) > Table -> ExtendedPM1bControlBlockAddress.Address ) )
				MappedRegister = reinterpret_cast <void *> ( CurrentKVMapPages [ I ] + ( Table -> ExtendedPM1bControlBlockAddress.Address & 0x00000FFF ) );
			
		}
		
		if ( MappedRegister == NULL )
		{
			
			MappedRegister = mm_kvmap ( reinterpret_cast <void *> ( Table -> ExtendedPM1bControlBlockAddress.Address ), 0x1000, MM::Paging::PageTable :: Flags_NoCache );
			
			if ( MappedRegister == NULL )
			{
				
				while ( CurrentKVMapPages.Length () != 0 )
					mm_kvunmap ( reinterpret_cast <void *> ( CurrentKVMapPages.Pop () ) );
					
				mm_kvunmap ( Table );
				
				* Status = kACPIStatus_Failure_System_OutOfMemory;
			
				return;
				
			}
			
			CurrentKVMapPages.Push ( reinterpret_cast <uint32_t> ( MappedRegister ) & 0xFFFFF000 );
			
		}
		
		Table -> ExtendedPM1bControlBlockAddress.Address = reinterpret_cast <uint64_t> ( MappedRegister );
		
	}
	
	// ExtendedPM2ControlBlockAddress
	
	if ( ( Table -> PM2ControlBlockAddress == 0 ) && ( Table -> ExtendedPM2ControlBlockAddress.Address != 0 ) && ( Table -> ExtendedPM2ControlBlockAddress.AddressSpaceID == kACPIAddress_AddressSpaceID_Memory ) )
	{
		
		void * MappedRegister = NULL;
		
		for ( uint32_t I = 0; I < CurrentKVMapPages.Length (); I ++ )
		{
			
			uint32_t PAdd = MM::Paging::PageTable :: KernelVirtualToPhysical ( CurrentKVMapPages [ I ] );
			
			if ( ( PAdd <= Table -> ExtendedPM2ControlBlockAddress.Address ) && ( ( PAdd + 0x1000 ) > Table -> ExtendedPM2ControlBlockAddress.Address ) )
				MappedRegister = reinterpret_cast <void *> ( CurrentKVMapPages [ I ] + ( Table -> ExtendedPM2ControlBlockAddress.Address & 0x00000FFF ) );
			
		}
		
		if ( MappedRegister == NULL )
		{
			
			MappedRegister = mm_kvmap ( reinterpret_cast <void *> ( Table -> ExtendedPM2ControlBlockAddress.Address ), 0x1000, MM::Paging::PageTable :: Flags_NoCache );
			
			if ( MappedRegister == NULL )
			{
				
				while ( CurrentKVMapPages.Length () != 0 )
					mm_kvunmap ( reinterpret_cast <void *> ( CurrentKVMapPages.Pop () ) );
					
				mm_kvunmap ( Table );
				
				* Status = kACPIStatus_Failure_System_OutOfMemory;
			
				return;
				
			}
			
			CurrentKVMapPages.Push ( reinterpret_cast <uint32_t> ( MappedRegister ) & 0xFFFFF000 );
			
		}
		
		Table -> ExtendedPM2ControlBlockAddress.Address = reinterpret_cast <uint64_t> ( MappedRegister );
		
	}
	
	// ExtendedPMTimerBlockAddress
	
	if ( ( Table -> PMTimerBlockAddress == 0 ) && ( Table -> ExtendedPMTimerBlockAddress.Address != 0 ) && ( Table -> ExtendedPMTimerBlockAddress.AddressSpaceID == kACPIAddress_AddressSpaceID_Memory ) )
	{
		
		void * MappedRegister = NULL;
		
		for ( uint32_t I = 0; I < CurrentKVMapPages.Length (); I ++ )
		{
			
			uint32_t PAdd = MM::Paging::PageTable :: KernelVirtualToPhysical ( CurrentKVMapPages [ I ] );
			
			if ( ( PAdd <= Table -> ExtendedPMTimerBlockAddress.Address ) && ( ( PAdd + 0x1000 ) > Table -> ExtendedPMTimerBlockAddress.Address ) )
				MappedRegister = reinterpret_cast <void *> ( CurrentKVMapPages [ I ] + ( Table -> ExtendedPMTimerBlockAddress.Address & 0x00000FFF ) );
			
		}
		
		if ( MappedRegister == NULL )
		{
			
			MappedRegister = mm_kvmap ( reinterpret_cast <void *> ( Table -> ExtendedPMTimerBlockAddress.Address ), 0x1000, MM::Paging::PageTable :: Flags_NoCache );
			
			if ( MappedRegister == NULL )
			{
				
				while ( CurrentKVMapPages.Length () != 0 )
					mm_kvunmap ( reinterpret_cast <void *> ( CurrentKVMapPages.Pop () ) );
					
				mm_kvunmap ( Table );
				
				* Status = kACPIStatus_Failure_System_OutOfMemory;
			
				return;
				
			}
			
			CurrentKVMapPages.Push ( reinterpret_cast <uint32_t> ( MappedRegister ) & 0xFFFFF000 );
			
		}
		
		Table -> ExtendedPMTimerBlockAddress.Address = reinterpret_cast <uint64_t> ( MappedRegister );
		
	}
	
	// ExtendedGPE0BlockAddress
	
	if ( ( Table -> GPE0BlockAddress == 0 ) && ( Table -> ExtendedGPE0BlockAddress.Address != 0 ) && ( Table -> ExtendedGPE0BlockAddress.AddressSpaceID == kACPIAddress_AddressSpaceID_Memory ) )
	{
		
		void * MappedRegister = NULL;
		
		for ( uint32_t I = 0; I < CurrentKVMapPages.Length (); I ++ )
		{
			
			uint32_t PAdd = MM::Paging::PageTable :: KernelVirtualToPhysical ( CurrentKVMapPages [ I ] );
			
			if ( ( PAdd <= Table -> ExtendedGPE0BlockAddress.Address ) && ( ( PAdd + 0x1000 ) > Table -> ExtendedGPE0BlockAddress.Address ) )
				MappedRegister = reinterpret_cast <void *> ( CurrentKVMapPages [ I ] + ( Table -> ExtendedGPE0BlockAddress.Address & 0x00000FFF ) );
			
		}
		
		if ( MappedRegister == NULL )
		{
			
			MappedRegister = mm_kvmap ( reinterpret_cast <void *> ( Table -> ExtendedGPE0BlockAddress.Address ), 0x1000, MM::Paging::PageTable :: Flags_NoCache );
			
			if ( MappedRegister == NULL )
			{
				
				while ( CurrentKVMapPages.Length () != 0 )
					mm_kvunmap ( reinterpret_cast <void *> ( CurrentKVMapPages.Pop () ) );
					
				mm_kvunmap ( Table );
				
				* Status = kACPIStatus_Failure_System_OutOfMemory;
			
				return;
				
			}
			
			CurrentKVMapPages.Push ( reinterpret_cast <uint32_t> ( MappedRegister ) & 0xFFFFF000 );
			
		}
		
		Table -> ExtendedGPE0BlockAddress.Address = reinterpret_cast <uint64_t> ( MappedRegister );
		
	}
	
	// ExtendedGPE1BlockAddress
	
	if ( ( Table -> GPE1BlockAddress == 0 ) && ( Table -> ExtendedGPE1BlockAddress.Address != 0 ) && ( Table -> ExtendedGPE1BlockAddress.AddressSpaceID == kACPIAddress_AddressSpaceID_Memory ) )
	{
		
		void * MappedRegister = NULL;
		
		for ( uint32_t I = 0; I < CurrentKVMapPages.Length (); I ++ )
		{
			
			uint32_t PAdd = MM::Paging::PageTable :: KernelVirtualToPhysical ( CurrentKVMapPages [ I ] );
			
			if ( ( PAdd <= Table -> ExtendedGPE1BlockAddress.Address ) && ( ( PAdd + 0x1000 ) > Table -> ExtendedGPE1BlockAddress.Address ) )
				MappedRegister = reinterpret_cast <void *> ( CurrentKVMapPages [ I ] + ( Table -> ExtendedGPE1BlockAddress.Address & 0x00000FFF ) );
			
		}
		
		if ( MappedRegister == NULL )
		{
			
			MappedRegister = mm_kvmap ( reinterpret_cast <void *> ( Table -> ExtendedGPE1BlockAddress.Address ), 0x1000, MM::Paging::PageTable :: Flags_NoCache );
			
			if ( MappedRegister == NULL )
			{
				
				while ( CurrentKVMapPages.Length () != 0 )
					mm_kvunmap ( reinterpret_cast <void *> ( CurrentKVMapPages.Pop () ) );
					
				mm_kvunmap ( Table );
				
				* Status = kACPIStatus_Failure_System_OutOfMemory;
			
				return;
				
			}
			
			CurrentKVMapPages.Push ( reinterpret_cast <uint32_t> ( MappedRegister ) & 0xFFFFF000 );
			
		}
		
		Table -> ExtendedGPE1BlockAddress.Address = reinterpret_cast <uint64_t> ( MappedRegister );
		
	}
	
	// SleepControlRegister
	
	if ( ( Table -> SleepControlRegister.Address != 0 ) && ( Table -> SleepControlRegister.AddressSpaceID == kACPIAddress_AddressSpaceID_Memory ) )
	{
		
		void * MappedRegister = NULL;
		
		for ( uint32_t I = 0; I < CurrentKVMapPages.Length (); I ++ )
		{
			
			uint32_t PAdd = MM::Paging::PageTable :: KernelVirtualToPhysical ( CurrentKVMapPages [ I ] );
			
			if ( ( PAdd <= Table -> SleepControlRegister.Address ) && ( ( PAdd + 0x1000 ) > Table -> SleepControlRegister.Address ) )
				MappedRegister = reinterpret_cast <void *> ( CurrentKVMapPages [ I ] + ( Table -> SleepControlRegister.Address & 0x00000FFF ) );
			
		}
		
		if ( MappedRegister == NULL )
		{
			
			MappedRegister = mm_kvmap ( reinterpret_cast <void *> ( Table -> SleepControlRegister.Address ), 0x1000, MM::Paging::PageTable :: Flags_NoCache );
			
			if ( MappedRegister == NULL )
			{
				
				while ( CurrentKVMapPages.Length () != 0 )
					mm_kvunmap ( reinterpret_cast <void *> ( CurrentKVMapPages.Pop () ) );
					
				mm_kvunmap ( Table );
				
				* Status = kACPIStatus_Failure_System_OutOfMemory;
			
				return;
				
			}
			
			CurrentKVMapPages.Push ( reinterpret_cast <uint32_t> ( MappedRegister ) & 0xFFFFF000 );
			
		}
		
		Table -> SleepControlRegister.Address = reinterpret_cast <uint64_t> ( MappedRegister );
		
	}
	
	// SleepStatusRegister
	
	if ( ( Table -> SleepStatusRegister.Address != 0 ) && ( Table -> SleepStatusRegister.AddressSpaceID == kACPIAddress_AddressSpaceID_Memory ) )
	{
		
		void * MappedRegister = NULL;
		
		for ( uint32_t I = 0; I < CurrentKVMapPages.Length (); I ++ )
		{
			
			uint32_t PAdd = MM::Paging::PageTable :: KernelVirtualToPhysical ( CurrentKVMapPages [ I ] );
			
			if ( ( PAdd <= Table -> SleepStatusRegister.Address ) && ( ( PAdd + 0x1000 ) > Table -> SleepStatusRegister.Address ) )
				MappedRegister = reinterpret_cast <void *> ( CurrentKVMapPages [ I ] + ( Table -> SleepStatusRegister.Address & 0x00000FFF ) );
			
		}
		
		if ( MappedRegister == NULL )
		{
			
			MappedRegister = mm_kvmap ( reinterpret_cast <void *> ( Table -> SleepStatusRegister.Address ), 0x1000, MM::Paging::PageTable :: Flags_NoCache );
			
			if ( MappedRegister == NULL )
			{
				
				while ( CurrentKVMapPages.Length () != 0 )
					mm_kvunmap ( reinterpret_cast <void *> ( CurrentKVMapPages.Pop () ) );
					
				mm_kvunmap ( Table );
				
				* Status = kACPIStatus_Failure_System_OutOfMemory;
			
				return;
				
			}
			
			CurrentKVMapPages.Push ( reinterpret_cast <uint32_t> ( MappedRegister ) & 0xFFFFF000 );
			
		}
		
		Table -> SleepStatusRegister.Address = reinterpret_cast <uint64_t> ( MappedRegister );
		
	}
	
	Validated = true;
	
	* Status = kACPIStatus_Success;
	
};

bool HW::ACPI::FADT :: Valid ()
{
	
	return Validated;
	
};

uint8_t HW::ACPI::FADT :: GetPreferredPMProfile ( uint32_t * Status )
{
	
	if ( ! Validated )
	{
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return 0;
		
	}
	
	uint8_t Profile = Table -> PreferredPMProfile;
	
	* Status = kACPIStatus_Success;
	
	return Profile;
	
};

uint16_t HW::ACPI::FADT :: GetACPISystemControlInterrupt ( uint32_t * Status )
{
	
	if ( ! Validated )
	{
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return 0;
		
	}
	
	uint16_t Interrupt = Table -> SystemControlInterrupt;
	
	* Status = kACPIStatus_Success;
	
	return Interrupt;
	
};

uint32_t HW::ACPI::FADT :: GetSMICommandPort ( uint32_t * Status )
{

	
	if ( ! Validated )
	{
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return 0;
		
	}
	
	uint32_t Port = Table -> SMICommandPort;
	
	* Status = kACPIStatus_Success;
	
	return Port;
	
};

void HW::ACPI::FADT :: WriteACPIEnable ( bool Enable, uint32_t * Status )
{
	
	if ( ! Validated )
	{
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return;
		
	}
	
	uint8_t Command;
	uint32_t Port = Table -> SMICommandPort;
	
	if ( Enable )
		Command = Table -> ACPIEnableCommand;
	else
		Command = Table -> ACPIDisableCommand;
	
	if ( Port == 0 ) // SMI not supported.
		return;
	
	HW::CPU::IO :: Out8 ( Port, Command );
	
	* Status = kACPIStatus_Success;
	
};

HW::ACPI :: ACPIAddress HW::ACPI::FADT :: GetPM1aEventBlockAddress ( uint32_t * Status )
{
	
	ACPIAddress Address;
	
	if ( ! Validated )
	{
		
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
	
	* Status = kACPIStatus_Success;
	
	return Address;
	
};

HW::ACPI :: ACPIAddress HW::ACPI::FADT :: GetPM1bEventBlockAddress ( uint32_t * Status )
{
	
	ACPIAddress Address;
	
	if ( ! Validated )
	{
		
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
	
	* Status = kACPIStatus_Success;
	
	return Address;
	
};

uint8_t HW::ACPI::FADT :: GetPM1EventBlockByteCount ( uint32_t * Status )
{
	
	if ( ! Validated )
	{
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return 0;
		
	}
	
	uint8_t Count = Table -> PM1EventBlockLength;
	
	* Status = kACPIStatus_Success;
	
	return Count;
	
};

HW::ACPI :: ACPIAddress HW::ACPI::FADT :: GetPM1aControlBlockAddress ( uint32_t * Status )
{
	
	ACPIAddress Address;
	
	if ( ! Validated )
	{
		
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
	
	* Status = kACPIStatus_Success;
	
	return Address;
	
};

HW::ACPI :: ACPIAddress HW::ACPI::FADT :: GetPM1bControlBlockAddress ( uint32_t * Status )
{
	
	ACPIAddress Address;
	
	if ( ! Validated )
	{
		
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
	
	* Status = kACPIStatus_Success;
	
	return Address;
	
};

uint8_t HW::ACPI::FADT :: GetPM1ControlBlockByteCount ( uint32_t * Status )
{
	
	if ( ! Validated )
	{
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return 0;
		
	}
	
	uint8_t Count = Table -> PM1ControlBlockLength;
	
	* Status = kACPIStatus_Success;
	
	return Count;
	
};

HW::ACPI :: ACPIAddress HW::ACPI::FADT :: GetPM2ControlBlockAddress ( uint32_t * Status )
{
	
	ACPIAddress Address;
	
	if ( ! Validated )
	{
		
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
	
	* Status = kACPIStatus_Success;
	
	return Address;
	
};

uint8_t HW::ACPI::FADT :: GetPM2ControlBlockByteCount ( uint32_t * Status )
{
	
	if ( ! Validated )
	{
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return 0;
		
	}
	
	uint8_t Count = Table -> PM2ControlBlockLength;
	
	* Status = kACPIStatus_Success;
	
	return Count;
	
};

HW::ACPI :: ACPIAddress HW::ACPI::FADT :: GetPMTimerBlockAddress ( uint32_t * Status )
{
	
	ACPIAddress Address;
	
	if ( ! Validated )
	{
		
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
	
	* Status = kACPIStatus_Success;
	
	return Address;
	
};

uint8_t HW::ACPI::FADT :: GetPMTimerBlockByteCount ( uint32_t * Status )
{
	
	if ( ! Validated )
	{
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return 0;
		
	}
	
	uint8_t Count = Table -> PMTimerBlockLength;
	
	* Status = kACPIStatus_Success;
	
	return Count;
	
};

HW::ACPI :: ACPIAddress HW::ACPI::FADT :: GetGeneralPurposeEvent0BlockAddress ( uint32_t * Status )
{
	
	ACPIAddress Address;
	
	if ( ! Validated )
	{
		
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
	
	* Status = kACPIStatus_Success;
	
	return Address;
	
};

HW::ACPI :: ACPIAddress HW::ACPI::FADT :: GetGeneralPurposeEvent1BlockAddress ( uint32_t * Status )
{
	
	ACPIAddress Address;
	
	if ( ! Validated )
	{
		
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
	
	* Status = kACPIStatus_Success;
	
	return Address;
	
};

uint8_t HW::ACPI::FADT :: GetGeneralPurposeEvent1Base ( uint32_t * Status )
{
	
	if ( ! Validated )
	{
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return 0;
		
	}
	
	uint8_t Base = Table -> GPE1Base;
	
	* Status = kACPIStatus_Success;
	
	return Base;
	
};

HW::ACPI :: ACPIAddress HW::ACPI::FADT :: GetResetRegisterAddress ( uint32_t * Status )
{
	
	ACPIAddress Address;
	
	if ( ! Validated )
	{
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return Address;
		
	}
	
	Address = Table -> ResetRegister;
	
	* Status = kACPIStatus_Success;
	
	return Address;
	
};

uint8_t HW::ACPI::FADT :: GetResetValue ( uint32_t * Status )
{
	
	if ( ! Validated )
	{
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return 0;
		
	}
	
	uint8_t Value = Table -> ResetValue;
	
	* Status = kACPIStatus_Success;
	
	return Value;
	
};

uint16_t HW::ACPI::FADT :: GetIAPCFlags ( uint32_t * Status )
{
	
	if ( ! Validated )
	{
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return 0;
		
	}
	
	uint32_t Value = Table -> IAPCFlags;
	
	* Status = kACPIStatus_Success;
	
	return Value;
	
};

uint32_t HW::ACPI::FADT :: GetFixedFeatureFlags ( uint32_t * Status )
{
	
	if ( ! Validated )
	{
		
		* Status = kACPIStatus_Failure_InvalidTable;
		
		return 0;
		
	}
	
	uint32_t Value = Table -> FixedFlags;
	
	* Status = kACPIStatus_Success;
	
	return Value;
	
};
