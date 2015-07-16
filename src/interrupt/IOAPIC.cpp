#include <interrupt/IOAPIC.h>

#include <hw/acpi/MADT.h>

#include <system/func/Panic.h>
#include <system/func/KPrintF.h>

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
				
				IOAPICVirtual = reinterpret_cast <uint32_t> ( VBaseAddresses [ J ] ) | ( Physical & 0xFFF );
				Allocated = true;
				
				break;
				
			}
			
		}
		
		if ( ! Allocated )
		{
			
			uint32_t VPage = reinterpret_cast <uint32_t> ( mm_kvmap ( reinterpret_cast <void *> ( Physical & 0xFFFFF000 ), 0x1000, MM::Paging::PageTable :: Flags_NoCache | MM::Paging::PageTable :: Flags_Writeable ) );
			
			if ( VPage == reinterpret_cast <uint32_t> ( static_cast <void *> ( NULL ) ) )
				KPANIC ( "Failed to allocate virtual space for I/O APIC Mapping!" );
			
			VBaseAddresses.Push ( VPage );
			
			IOAPICVirtual = VPage | ( Physical & 0xFFF );
			
		}
		
		IOAPICInfo Info;
		
		Info.BaseAddress = reinterpret_cast <void *> ( IOAPICVirtual );
		Info.GlobalSystemInterruptBase = HW::ACPI::MADT :: GetIOAPICGlobalSystemInterruptBase ( I );
		Info.ID = HW::ACPI::MADT :: GetIOAPICID ( I );
		
		IOAPICs -> Push ( Info );
		
		system_func_kprintf ( "IOAPIC (%u): Base: %h, ID: %h, GSIBase: %h\n", I, reinterpret_cast <uint32_t> ( Info.BaseAddress ), Info.ID, Info.GlobalSystemInterruptBase );
		
	}
	
	for ( I = 0; I < HW::ACPI::MADT :: GetInterruptSourceOverrideCount (); I ++ )
	{
		
		system_func_kprintf ( "Interrupt source override: BusSource: %h, IRQSource: %h, Flags: %h\n", HW::ACPI::MADT :: GetInterruptSourceOverrideBusSource ( I ), HW::ACPI::MADT :: GetInterruptSourceOverrideIRQSource ( I ), HW::ACPI::MADT :: GetInterruptSourceOverrideBus ( I ) );
		
	}
	
};

uint32_t Interrupt::IOAPIC :: ReadRegister ( void * BaseAddress, uint32_t Register )
{
	
	reinterpret_cast <uint32_t *> ( BaseAddress ) [ 0 ] = Register & 0xFF;
	return reinterpret_cast <uint32_t *> ( BaseAddress ) [ 4 ];
	
};

void Interrupt::IOAPIC :: WriteRegister ( void * BaseAddress, uint32_t Register, uint32_t Value )
{
	
	reinterpret_cast <uint32_t *> ( BaseAddress ) [ 0 ] = Register & 0xFF;
	reinterpret_cast <uint32_t *> ( BaseAddress ) [ 4 ] = Value;
	
};
