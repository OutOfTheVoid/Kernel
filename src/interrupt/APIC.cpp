#include <interrupt/APIC.h>
#include <interrupt/IState.h>
#include <interrupt/PIC.h>

#include <hw/cpu/CPUID.h>
#include <hw/cpu/MSR.h>

#include <hw/acpi/MADT.h>

#include <system/func/panic.h>

#include <cpputil/linkage.h>

C_LINKAGE uint32_t interrupt_APIC_MMIOLocation;

bool Interrupt::APIC :: Availible = true;

void Interrupt::APIC :: Init ()
{
	
	if ( ! HasAPIC () )
		system_func_panic ( "System does not report an APIC!" );
	
	// Remap PIC to unused interrupt numbers.
	Interrupt::PIC :: Remap ( 0x50, 0x58 );
	
	for ( uint32_t i = 0; i < 15; i ++ )
		Interrupt::PIC :: SetIRQEnabled ( i, false );
	
	Interrupt::PIC :: Disable ();
	
	EnableAPIC ();
	
};

void Interrupt::APIC :: SetAPICBaseAddress ( uint32_t Base )
{
	
	uint32_t EAX = ( Base & 0xFFFFF100 ) | HW::CPU::MSR :: kMSR_APIC_ENABLE;
	uint32_t EDX = 0;
	
	HW::CPU::MSR :: SetMSR ( HW::CPU::MSR :: kMSR_APIC_BASE, & EAX, & EDX );
	
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


