#include <hw/acpi/ACPI.h>
#include <hw/acpi/RSDP.h>
#include <hw/acpi/RSDT.h>
#include <hw/acpi/XSDT.h>
#include <hw/acpi/MADT.h>
//#include <hw/acpi/SRAT.h>

#include <system/func/kprintf.h>

#include <KernelDef.h>

bool HW::ACPI :: StaticInit ()
{
	
#ifdef KSTARTUP_DEBUG
		system_func_kprintf ( "ACPI Static startup:\nLocating RSDP...\n" );
#endif
	
	RSDP :: Locate ();
	
	if ( ! RSDP :: Found () )
	{
		
#ifdef KSTARTUP_DEBUG
		system_func_kprintf ( "ACPI Static startup failed finding RSDP!\n" );
#endif
		
		return false;
		
	}
	
	#ifdef KSTARTUP_DEBUG
		system_func_kprintf ( "RSDP Found!\nValidating SDT...\n" );
	#endif
	
	if ( RSDP :: GetACPIRevision () == RSDP :: kACPI_Revision_1 )
	{
		
		RSDT :: Init ( RSDP :: GetSDTPointer () );
		
		if ( ! RSDT :: Valid () )
		{
			
			#ifdef KSTARTUP_DEBUG
				system_func_kprintf ( "ACPI Static startup detected invalid RSDT!\n" );
			#endif
				
			return false;
			
		}
		
		#ifdef KSTARTUP_DEBUG
			system_func_kprintf ( "RSDT Validated!\nParsing MADT...\n" );
		#endif
		
	}
	else
	{
		
		XSDT :: Init ( RSDP :: GetSDTPointer () );
		
		if ( ! XSDT :: Valid () )
		{
			
			#ifdef KSTARTUP_DEBUG
				system_func_kprintf ( "ACPI Static startup detected invalid XSDT!\n" );
			#endif
				
			return false;
			
		}
		
		#ifdef KSTARTUP_DEBUG
			system_func_kprintf ( "XSDT Validated!\nParsing MADT...\n" );
		#endif
		
	}
	
	MADT :: Init ();
	
	if ( ! MADT :: Valid () )
	{
		
		#ifdef KSTARTUP_DEBUG
			system_func_kprintf ( "MADT Not valid!\nParsing SRAT..." );
		#endif
		
	}
	else
	{
		
		
		#ifdef KSTARTUP_DEBUG
			system_func_kprintf ( "MADT Validated!\nParsing SRAT...\n" );
		#endif
		
	}
	
	/*SRAT :: Init ();
	
	if ( ! SRAT :: Valid () )
	{
		
		#ifdef KSTARTUP_DEBUG
			system_func_kprintf ( "SRAT Not valid!\n" );
		#endif
		
	}
	else
	{
		
		
		#ifdef KSTARTUP_DEBUG
			system_func_kprintf ( "SRAT Validated!\n" );
		#endif
		
	}*/
	
	system_func_kprintf ( "ACPI Static startup successful!\n\n" );
	
	return true;
	
};
