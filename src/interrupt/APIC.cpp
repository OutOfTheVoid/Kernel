#include <interrupt/APIC.h>
#include <interrupt/IState.h>
#include <interrupt/PIC.h>

#include <hw/cpu/CPUID.h>
#include <hw/cpu/MSR.h>

#include <hw/acpi/MADT.h>

#include <system/func/Panic.h>
#include <system/func/KPrintF.h>

#include <mm/KVMap.h>

#include <mm/paging/PageTable.h>

#include <cpputil/linkage.h>

C_LINKAGE uint32_t interrupt_APIC_MMIOLocation;

bool Interrupt::APIC :: Availible = true;
		
uint32_t Interrupt::APIC :: Base = 0;
uint32_t * Interrupt::APIC :: BaseVirtual = NULL;

void Interrupt::APIC :: Init ()
{
	
	if ( ! HasAPIC () )
		system_func_panic ( "System does not report an APIC!" );
	
	// Remap PIC to unused interrupt numbers.
	Interrupt::PIC :: Remap ( 0x50, 0x58 );
	
	for ( uint32_t i = 0; i < 15; i ++ )
		Interrupt::PIC :: SetIRQEnabled ( i, false );
	
	Interrupt::PIC :: Disable ();
	
	system_func_kprintf ( "APIC Base address: %h\n", GetAPICBaseAddress () );
	
	EnableAPIC ();
	
	BaseVirtual = reinterpret_cast <uint32_t *> ( mm_kvmap ( reinterpret_cast <void *> ( Base ), 0x1000, MM::Paging::PageTable :: Flags_NoCache | MM::Paging::PageTable :: Flags_Writeable ) );
	
	if ( BaseVirtual == NULL )
		KPANIC ( "Failed to allocate virtual memory to map APIC!" );
	
};

void Interrupt::APIC :: ReadRegister ( uint32_t Offset, uint32_t * DataOut, uint32_t DWordLength )
{
	
	if ( BaseVirtual == NULL )
		KPANIC ( "Attempt to read APIC register without first mapping it into memory!" );
	
	for ( uint32_t I = 0; I < DWordLength; I ++ )
		DataOut [ I ] = BaseVirtual [ ( Offset >> 2 ) + I ];
	
};

void Interrupt::APIC :: WriteRegister ( uint32_t Offset, uint32_t * DataIn, uint8_t DWordLength )
{
	
	if ( BaseVirtual == NULL )
		KPANIC ( "Attempt to write APIC register without first mapping it into memory!" );
	
	for ( uint32_t I = 0; I < DWordLength; I ++ )
		BaseVirtual [ ( Offset >> 2 ) + I ] = DataIn [ I ];
	
};

void Interrupt::APIC :: SetAPICBaseAddress ( uint32_t Base )
{
	
	uint32_t EAX = ( Base & 0xFFFFF100 ) | HW::CPU::MSR :: kMSR_APIC_ENABLE;
	uint32_t EDX = 0;
	
	HW::CPU::MSR :: SetMSR ( HW::CPU::MSR :: kMSR_APIC_BASE, & EAX, & EDX );
	
	APIC :: Base = Base;
	
	if ( BaseVirtual != NULL )
		MM::Paging::PageTable :: SetKernelMapping ( reinterpret_cast <uint32_t> ( BaseVirtual ), Base, MM::Paging::PageTable :: Flags_NoCache | MM::Paging::PageTable :: Flags_Writeable | MM::Paging::PageTable :: Flags_Present );
	
};

uint32_t Interrupt::APIC :: GetAPICBaseAddress ()
{
	
	if ( HW::ACPI::MADT :: Valid () )
		return HW::ACPI::MADT :: GetAPICBaseAddress ();
	
	uint32_t EAX;
	uint32_t EDX;
	
	HW::CPU::MSR :: GetMSR ( HW::CPU::MSR :: kMSR_APIC_BASE, & EAX, & EDX );
	
	return EAX & 0xFFFFF000;
	
};

void Interrupt::APIC :: EnableAPIC ()
{
	
	SetAPICBaseAddress ( GetAPICBaseAddress () );
	
};

bool Interrupt::APIC :: HasAPIC ()
{
	
	if ( Availible == false )
		return false;
	
	if ( HW::ACPI::MADT :: Valid () )
		return true;
	
	uint32_t CPUIDRegs [ 4 ];
	
	HW::CPU::CPUID :: GetCPUID ( 0x01, CPUIDRegs );
	
	Availible = CPUIDRegs [ HW::CPU::CPUID :: kRegister_EDX ] & HW::CPU::CPUID :: kDFlag_APIC;
	return Availible;
	
};


