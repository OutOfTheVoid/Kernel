#include <interrupt/IOAPIC.h>

#include <hw/acpi/MADT.h>

#include <system/func/Panic.h>

#include <mm/KMalloc.h>
#include <mm/KVMap.h>

#include <mm/paging/PageTable.h>

Vector <Interrupt::IOAPIC :: IOAPICInfo> * Interrupt::IOAPIC :: IOAPICs = NULL;

void Interrupt::IOAPIC :: Init ()
{
	
	Vector <uint32_t> VBaseAddresses;
	
	uint32_t I;
	uint32_t J;
	
	uint32_t IOAPICPhysical;
	uint32_t IOAPICVirtual = 0;
	
	IOAPICs = new Vector <IOAPICInfo> ();
	
	for ( I = 0; I < HW::ACPI::MADT :: GetIOAPICCount (); I ++ )
	{
		
		bool Allocated = false;
		
		uint32_t Physical = 0;
		
		IOAPICPhysical = HW::ACPI::MADT :: GetIOAPICBaseAddress ( I );
		
		for ( J = 0; J < VBaseAddresses.Length (); J ++ )
		{
			
			Physical = MM::Paging::PageTable :: KernelVirtualToPhysical ( reinterpret_cast <uint32_t> ( VBaseAddresses [ J ] ) );
			
			if ( ( Physical <= IOAPICPhysical ) && ( ( Physical + 0x1000 ) > IOAPICPhysical ) )
			{
				
				IOAPICVirtual = reinterpret_cast <uint32_t> ( VBaseAddresses [ J ] ) | ( IOAPICPhysical & 0xFFF );
				Allocated = true;
				
				break;
				
			}
			
		}
		
		if ( ! Allocated )
		{
			
			uint32_t VPage = reinterpret_cast <uint32_t> ( mm_kvmap ( reinterpret_cast <void *> ( IOAPICPhysical & 0xFFFFF000 ), 0x1000, MM::Paging::PageTable :: Flags_NoCache | MM::Paging::PageTable :: Flags_Writeable ) );
			
			if ( VPage == reinterpret_cast <uint32_t> ( static_cast <void *> ( NULL ) ) )
				KPANIC ( "Failed to allocate virtual space for I/O APIC Mapping!" );
			
			VBaseAddresses.Push ( VPage );
			
			IOAPICVirtual = VPage | ( IOAPICPhysical & 0xFFF );
			
		}
		
		uint32_t Version = ReadRegister ( reinterpret_cast <void *> ( IOAPICVirtual ), kRegister_Version );
		uint32_t ID = ReadRegister ( reinterpret_cast <void *> ( IOAPICVirtual ), kRegister_ID );
		
		IOAPICInfo Info;
		
		Info.BaseAddress = reinterpret_cast <void *> ( IOAPICVirtual );
		Info.GlobalSystemInterruptBase = HW::ACPI::MADT :: GetIOAPICGlobalSystemInterruptBase ( I );
		Info.ID = HW::ACPI::MADT :: GetIOAPICID ( I );
		Info.GlobalSystemInterruptCount = ( ( Version >> 16 ) & 0xFF ) + 1;
		Info.Version = Version & 0xFF;
		Info.AllocationBitmap = 0;
		
		if ( ( Info.ID == ( ( ID >> 24 ) & 0xFF ) ) )
			IOAPICs -> Push ( Info );
		
	}
	
};

bool Interrupt::IOAPIC :: TryAllocateGlobalSystemInterrupt ( uint32_t Interrupt )
{
	
	uint32_t I;
	
	void * BaseAddress = NULL;
	
	for ( I = 0; I < IOAPICs -> Length (); I ++ )
	{
		
		uint32_t IOAPICGSIBase = ( * IOAPICs ) [ I ].GlobalSystemInterruptBase;
		
		if ( ( IOAPICGSIBase <= Interrupt ) && ( IOAPICGSIBase + ( * IOAPICs ) [ I ].GlobalSystemInterruptCount > Interrupt ) )
		{
			
			uint32_t Bit = 1 << ( Interrupt - IOAPICGSIBase );
			
			if ( ( ( * IOAPICs ) [ I ].AllocationBitmap & Bit ) == 0 )
			{
				
				( * IOAPICs ) [ I ].AllocationBitmap |= Bit;
				
				return true;
				
			}

			return false;
			
		}
	
	}
	
	if ( BaseAddress == NULL )
		KPANIC ( "No I/O APIC found with matching global system interrupt range containing requested interrupt #%i!" );

	return false;

};

void Interrupt::IOAPIC :: FreeGlobalSystemInterrupt ( uint32_t Interrupt )
{
	
	uint32_t I;
	
	void * BaseAddress = NULL;
	
	for ( I = 0; I < IOAPICs -> Length (); I ++ )
	{
		
		uint32_t IOAPICGSIBase = ( * IOAPICs ) [ I ].GlobalSystemInterruptBase;
		
		if ( ( IOAPICGSIBase <= Interrupt ) && ( IOAPICGSIBase + ( * IOAPICs ) [ I ].GlobalSystemInterruptCount > Interrupt ) )
		{
			
			uint32_t Bit = 1 << ( Interrupt - IOAPICGSIBase );

			( * IOAPICs ) [ I ].AllocationBitmap &= ~ Bit;
			
		}
		
	}
	
	if ( BaseAddress == NULL )
		KPANIC ( "No I/O APIC found with matching global system interrupt range containing requested interrupt #%i!" );

};

void Interrupt::IOAPIC :: DefineFixedRedirectionEntry ( uint32_t Interrupt, uint32_t TargetVector, uint8_t LAPICID, bool ActiveHigh, bool EdgeTriggered, bool InitiallyMasked )
{
	
	uint32_t I;
	uint32_t IOAPICGSIBase = 0;
	
	void * BaseAddress = NULL;
	
	for ( I = 0; I < IOAPICs -> Length (); I ++ )
	{
		
		uint32_t IOAPICGSIBase = ( * IOAPICs ) [ I ].GlobalSystemInterruptBase;
		
		if ( ( IOAPICGSIBase <= Interrupt ) && ( IOAPICGSIBase + ( * IOAPICs ) [ I ].GlobalSystemInterruptCount > Interrupt ) )
		{
			
			BaseAddress = ( * IOAPICs ) [ I ].BaseAddress;
			
			break;
			
		}
	
	}
	
	if ( BaseAddress == NULL )
		KPANIC ( "No I/O APIC found with matching global system interrupt range containing requested interrupt #%i!" );
	
	uint32_t EntryLow = ( TargetVector & 0xFF ) | kRedirectionEntry_Low_Deliveryode_Fixed | kRedirectionEntry_Low_DestinationMode_Physical | ( ActiveHigh ? kRedirectionEntry_Low_PinPolarity_ActiveHigh : kRedirectionEntry_Low_PinPolarity_ActiveLow ) | ( EdgeTriggered ? kRedirectionEntry_Low_TriggerMode_Edge : kRedirectionEntry_Low_TriggerMode_Level ) | ( InitiallyMasked ? kRedirectionEntry_Low_Mask_Set : kRedirectionEntry_Low_Mask_Clear );
	uint32_t EntryHigh = ( LAPICID << kRedirectionEntry_High_BitBase_PhysicalDestination ) | ( kRedirectionEntry_High_UnusedMask & ReadRegister ( BaseAddress, kRegister_BaseRedirectionEntry + 2 * ( Interrupt - IOAPICGSIBase ) + 1 ) );
	
	WriteRegister ( BaseAddress, kRegister_BaseRedirectionEntry + 2 * ( Interrupt - IOAPICGSIBase ), EntryLow );
	WriteRegister ( BaseAddress, kRegister_BaseRedirectionEntry + 2 * ( Interrupt - IOAPICGSIBase ) + 1, EntryHigh );
	
};

void Interrupt::IOAPIC :: DefineLowestPriorityRedirectionEntry ( uint32_t Interrupt, uint32_t TargetVector, bool ActiveHigh, bool EdgeTriggered, bool InitiallyMasked )
{
	
	uint32_t I;
	uint32_t IOAPICGSIBase = 0;
	
	void * BaseAddress = NULL;
	
	for ( I = 0; I < IOAPICs -> Length (); I ++ )
	{
		
		uint32_t IOAPICGSIBase = ( * IOAPICs ) [ I ].GlobalSystemInterruptBase;
		
		if ( ( IOAPICGSIBase <= Interrupt ) && ( IOAPICGSIBase + ( * IOAPICs ) [ I ].GlobalSystemInterruptCount > Interrupt ) )
		{
			
			BaseAddress = ( * IOAPICs ) [ I ].BaseAddress;
			
			break;
			
		}
	
	}
	
	if ( BaseAddress == NULL )
		KPANIC ( "No I/O APIC found with matching global system interrupt range containing requested interrupt #%i!", Interrupt );
	
	uint32_t EntryLow = ( TargetVector & 0xFF ) | kRedirectionEntry_Low_Deliveryode_LowestPriority | kRedirectionEntry_Low_DestinationMode_Physical | ( ActiveHigh ? kRedirectionEntry_Low_PinPolarity_ActiveHigh : kRedirectionEntry_Low_PinPolarity_ActiveLow ) | ( EdgeTriggered ? kRedirectionEntry_Low_TriggerMode_Edge : kRedirectionEntry_Low_TriggerMode_Level ) | ( InitiallyMasked ? kRedirectionEntry_Low_Mask_Set : kRedirectionEntry_Low_Mask_Clear );
	uint32_t EntryHigh = ( kRedirectionEntry_High_UnusedMask & ReadRegister ( BaseAddress, kRegister_BaseRedirectionEntry + 2 * ( Interrupt - IOAPICGSIBase ) + 1 ) );
	
	WriteRegister ( BaseAddress, kRegister_BaseRedirectionEntry + 2 * ( Interrupt - IOAPICGSIBase ), EntryLow );
	WriteRegister ( BaseAddress, kRegister_BaseRedirectionEntry + 2 * ( Interrupt - IOAPICGSIBase ) + 1, EntryHigh );
	
};

void Interrupt::IOAPIC :: SetRedirectionEntryEnabled ( uint32_t Interrupt, bool Enabled )
{
	
	uint32_t I;
	uint32_t IOAPICGSIBase = 0;
	
	void * BaseAddress = NULL;
	
	for ( I = 0; I < IOAPICs -> Length (); I ++ )
	{
		
		uint32_t IOAPICGSIBase = ( * IOAPICs ) [ I ].GlobalSystemInterruptBase;
		
		if ( ( IOAPICGSIBase <= Interrupt ) && ( IOAPICGSIBase + ( * IOAPICs ) [ I ].GlobalSystemInterruptCount > Interrupt ) )
		{
			
			BaseAddress = ( * IOAPICs ) [ I ].BaseAddress;
			
			break;
			
		}
	
	}
	
	if ( BaseAddress == NULL )
		KPANIC ( "No I/O APIC found with matching global system interrupt range containing requested interrupt #%i!", Interrupt );
	
	uint32_t EntryLow = ReadRegister ( BaseAddress, kRegister_BaseRedirectionEntry + 2 * ( Interrupt - IOAPICGSIBase ) );
	
	if ( Enabled )
	{
		
		EntryLow &= ~ kRedirectionEntry_Low_Mask_Set;
		WriteRegister ( BaseAddress, kRegister_BaseRedirectionEntry + 2 * ( Interrupt - IOAPICGSIBase ), EntryLow );
		
	}
	else
	{
		
		EntryLow |= kRedirectionEntry_Low_Mask_Set;
		WriteRegister ( BaseAddress, kRegister_BaseRedirectionEntry + 2 * ( Interrupt - IOAPICGSIBase ), EntryLow );
		
	}
	
};

uint32_t Interrupt::IOAPIC :: ReadRegister ( volatile void * BaseAddress, volatile uint32_t Register )
{
	
	* reinterpret_cast <volatile uint32_t *> ( BaseAddress ) = Register & 0xFF;
	return * reinterpret_cast <volatile uint32_t *> ( reinterpret_cast <volatile uint32_t> ( BaseAddress ) + 0x10 );
	
};

void Interrupt::IOAPIC :: WriteRegister ( volatile void * BaseAddress, volatile uint32_t Register, uint32_t Value )
{
	
	* reinterpret_cast <volatile uint32_t *> ( BaseAddress ) = Register & 0xFF;
	* reinterpret_cast <volatile uint32_t *> ( reinterpret_cast <volatile uint32_t> ( BaseAddress ) + 0x10 ) = Value;
	
};
