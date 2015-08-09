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

#include <interrupt/APIC.h>
#include <interrupt/IState.h>

#include <mt/synchronization/Spinlock.h>

HW::ACPI :: SCIHandlerHook * _hw_acpi_RootSCIHook = NULL;
MT::Synchronization::Spinlock :: Spinlock_t _hw_acpi_SCIHookLock = MT::Synchronization::Spinlock :: Initializer ();

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
	
	(void) Frame;
	
	system_func_kprintf ( "SCI!\n" );
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & _hw_acpi_SCIHookLock );
	
	if ( _hw_acpi_RootSCIHook != NULL )
	{
		
		bool Continue = true;
		
		while ( Continue )
		{
			
			SCIHandlerHook * Current = _hw_acpi_RootSCIHook;
			
			do
			{
				
				bool Remove = Current -> Handler ();
				
				if ( Remove )
				{
					
					if ( Current -> Next == Current )
					{
						
						_hw_acpi_RootSCIHook = NULL;
						
						break;
						
					}
					
					if ( Current == _hw_acpi_RootSCIHook )
					{
						
						_hw_acpi_RootSCIHook = Current -> Next;
						_hw_acpi_RootSCIHook -> Last = Current -> Last;
						Current -> Last -> Next = _hw_acpi_RootSCIHook;
						
						continue;
						
					}
					
				}
				
			}
			while ( Current -> Next != _hw_acpi_RootSCIHook );
			
			Continue = false;
			
		}
		
	}
	
	MT::Synchronization::Spinlock :: Release ( & _hw_acpi_SCIHookLock );
	
	Interrupt::APIC :: EndOfInterrupt ();
	
};

void HW::ACPI :: InitSCIHandlerHook ( SCIHandlerHook * Hook, bool ( * Handler ) () )
{
	
	Hook -> Next = NULL;
	Hook -> Last = NULL;
	
	Hook -> Handler = Handler;
	
};

void HW::ACPI :: AddSCIHandlerHook ( SCIHandlerHook * Hook )
{
	
	bool ReInt = Interrupt::IState :: ReadAndSetBlock ();
	MT::Synchronization::Spinlock :: SpinAcquire ( & _hw_acpi_SCIHookLock );
	
	if ( _hw_acpi_RootSCIHook == NULL )
	{
		
		_hw_acpi_RootSCIHook = Hook;
		
		Hook -> Next = Hook;
		Hook -> Last = Hook;
		
	}
	else
	{
		
		Hook -> Next = _hw_acpi_RootSCIHook;
		Hook -> Last = _hw_acpi_RootSCIHook -> Last;
		
		_hw_acpi_RootSCIHook -> Last -> Next = Hook;
		_hw_acpi_RootSCIHook -> Last = Hook;
		
	}
	
	MT::Synchronization::Spinlock :: Release ( & _hw_acpi_SCIHookLock );
	Interrupt::IState :: WriteBlock ( ReInt );
	
};

void HW::ACPI :: RemoveSCIHandlerHook ( SCIHandlerHook * Hook )
{
	
	bool ReInt = Interrupt::IState :: ReadAndSetBlock ();
	MT::Synchronization::Spinlock :: SpinAcquire ( & _hw_acpi_SCIHookLock );
	
	if ( Hook == _hw_acpi_RootSCIHook )
	{
		
		if ( Hook -> Next == Hook )
			_hw_acpi_RootSCIHook = NULL;
		else
		{
			
			Hook -> Next -> Last = Hook -> Last;
			Hook -> Last -> Next = Hook -> Next;
			
			_hw_acpi_RootSCIHook = Hook -> Next;
			
		}
		
	}
	else
	{
		
		Hook -> Next -> Last = Hook -> Last;
		Hook -> Last -> Next = Hook -> Next;
		
	}
	
	MT::Synchronization::Spinlock :: Release ( & _hw_acpi_SCIHookLock );
	Interrupt::IState :: WriteBlock ( ReInt );
	
};
