#include <hw/acpi/ACPI.h>
#include <hw/acpi/RSDP.h>
#include <hw/acpi/RSDT.h>
#include <hw/acpi/XSDT.h>
#include <hw/acpi/MADT.h>
#include <hw/acpi/SRAT.h>
#include <hw/acpi/FADT.h>
#include <hw/acpi/PMTimer.h>

#include <system/func/kprintf.h>

#include <KernelDef.h>

#include <hw/pc/ISA.h>

void HW::ACPI :: StaticInit ( uint32_t * Status )
{
	
	uint32_t SubStatus;
	
	RSDP :: Locate ();
	
	if ( ! RSDP :: Found () )
	{
		
		* Status = kACPIStatus_Failiure_NoACPI;
		
		return;
		
	}
	
	if ( RSDP :: GetACPIRevision () == RSDP :: kACPI_Revision_1 )
	{
		
		RSDT :: Init ( RSDP :: GetSDTPointer () );
		
		if ( ! RSDT :: Valid () )
		{
			
			* Status = kACPIStatus_Failure_InvalidTable;
			
			return;
			
		}
		
	}
	else
	{
		
		XSDT :: Init ( RSDP :: GetSDTPointer () );
		
		if ( ! XSDT :: Valid () )
		{
			
			* Status = kACPIStatus_Failure_InvalidTable;
			
			return;
			
		}
		
	}
	
	MADT :: Init ();
	SRAT :: Init ();
	
	FADT :: Init ( & SubStatus );
	
	* Status = kACPIStatus_Success;
	
};

void HW::ACPI :: InitInterrupts ( uint32_t * Status )
{
	
	uint32_t SubStatus;
	
	if ( FADT :: Valid () )
	{
		
		uint16_t Interrupt = FADT :: GetACPISystemControlInterrupt ( & SubStatus );
		
		if ( SubStatus == kACPIStatus_Success )
		{
			
			system_func_kprintf ( "System control IRQ: %u\n", static_cast <uint32_t> ( Interrupt ) );
			
			PC::ISA :: SetIRQHandler ( Interrupt, & SystemControlInterruptHandler );
			PC::ISA :: SetIRQEnabled ( Interrupt, true );
			
			* Status = kACPIStatus_Success;
			
		}
		else
		{
			
			system_func_kprintf ( "Failed to retreive SCI Number\n" );
			
			* Status = kACPIStatus_Failure_ResourceNotFound;
			
		}
		
	}
	
	if ( * Status == kACPIStatus_Success )
	{
		
		HW::ACPI::PMTimer :: Init ( & SubStatus );
		
		if ( SubStatus == kACPIStatus_Success )
			system_func_kprintf ( "PMTimer valid!\n" );
		
	}
	
};

void HW::ACPI :: Enable ( uint32_t * Status )
{
	
	uint32_t SubStatus;
	
	if ( FADT :: Valid () )
	{
		
		FADT :: WriteACPIEnable ( true, & SubStatus );
		
		if ( SubStatus != kACPIStatus_Success )
			* Status = kACPIStatus_Failure_ResourceNotFound;
		
	}
	else
		* Status = kACPIStatus_Failure_InvalidTable;
	
};

void HW::ACPI :: Disable ( uint32_t * Status )
{
	
	uint32_t SubStatus;
	
	if ( FADT :: Valid () )
	{
		
		FADT :: WriteACPIEnable ( false, & SubStatus );
		
		if ( SubStatus != kACPIStatus_Success )
			* Status = kACPIStatus_Failure_ResourceNotFound;
		
	}
	else
		* Status = kACPIStatus_Failure_InvalidTable;
	
};

void HW::ACPI :: SystemControlInterruptHandler ( Interrupt::InterruptHandlers :: ISRFrame * Frame )
{
	
	system_func_kprintf ( "SCI!\n" );
	
};
