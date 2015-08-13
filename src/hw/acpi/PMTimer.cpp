#include <hw/acpi/PMTimer.h>
#include <hw/acpi/FADT.h>

#include <hw/cpu/IO.h>

#include <mm/kvmap.h>

#include <interrupt/IState.h>

bool HW::ACPI::PMTimer :: Exist = false;

uint32_t HW::ACPI::PMTimer :: LastTValue = 0;
uint32_t HW::ACPI::PMTimer :: RegisterMask = 0;
uint64_t HW::ACPI::PMTimer :: Count = 0;

uint32_t HW::ACPI::PMTimer :: TimerAddressSpace = false;
uint32_t HW::ACPI::PMTimer :: TimerAddress = 0;

uint32_t HW::ACPI::PMTimer :: EventAddressA = 0;
uint32_t HW::ACPI::PMTimer :: EventAddressB = 0;
uint32_t HW::ACPI::PMTimer :: EventAddressSpaceA = 0;
uint32_t HW::ACPI::PMTimer :: EventAddressSpaceB = 0;

MT::Synchronization::Spinlock :: Spinlock_t HW::ACPI::PMTimer :: Lock = MT::Synchronization::Spinlock :: Initializer ();

HW::ACPI :: SCIHandlerHook HW::ACPI::PMTimer :: TimerIntHook;

void HW::ACPI::PMTimer :: Init ( uint32_t * Status )
{
	
	uint32_t SubStatus;
	
	bool FailA;
	
	* Status = kACPIStatus_Success;
	
	// Check that the FADT is valid before using it.
	
	if ( FADT :: Valid () )
	{
		
		// Is the PM Timer 32 or 24 bits wide?
		
		RegisterMask = ( ( FADT :: GetFixedFeatureFlags ( & SubStatus ) & FADT :: kFixedFeatureFlag_PMTimerExtended ) != 0 ) ? kRegisterMask_32 : kRegisterMask_24;
		
		if ( SubStatus != kACPIStatus_Success )
		{
			
			* Status = kACPIStatus_Failure_ResourceNotFound;
			
			return;
			
		}
		
		// Get the address of the PM Timer value register, and record it's "initial" value.
		
		ACPIAddress TimerBlockAddress = FADT :: GetPMTimerBlockAddress ( & SubStatus );
		
		if ( ( SubStatus != kACPIStatus_Success ) || ( TimerBlockAddress.Address == 0 ) )
		{
			
			* Status = kACPIStatus_Failure_ResourceNotFound;
			
			return;
			
		}
		
		switch ( TimerBlockAddress.AddressSpaceID )
		{
			
		case kACPIAddress_AddressSpaceID_Memory:
			{
				
				TimerAddress = TimerBlockAddress.Address;
				TimerAddressSpace= kACPIAddress_AddressSpaceID_Memory;
				
				LastTValue = ( * reinterpret_cast <volatile uint32_t *> ( TimerAddress ) ) & RegisterMask;
				
			}
			break;
			
		case kACPIAddress_AddressSpaceID_SystemIO:
			{
				
				TimerAddress = TimerBlockAddress.Address;
				TimerAddressSpace= kACPIAddress_AddressSpaceID_SystemIO;
				
				LastTValue = HW::CPU::IO :: In32 ( TimerAddress ) & RegisterMask;
				
			}
			break;
			
		default:
			
			* Status = kACPIStatus_Failure_UnsupportedAddressSpace;
			
			return;
			
			break;
			
		}
		
		if ( SubStatus != kACPIStatus_Success )
		{
			
			* Status = kACPIStatus_Failure_ResourceNotFound;
			
			return;
			
		}
		
		// Below deals with getting the PM1a/b event/status register address.
		
		ACPIAddress PM1aEventAddress = FADT :: GetPM1aEventBlockAddress ( & SubStatus );
		
		FailA = ( ( PM1aEventAddress.Address == 0 ) || ( SubStatus != kACPIStatus_Success ) );
		
		ACPIAddress PM1bEventAddress = FADT :: GetPM1bEventBlockAddress ( & SubStatus );
		
		if ( ( ( PM1bEventAddress.Address == 0 ) || ( SubStatus != kACPIStatus_Success ) ) && FailA )
		{
			
			* Status = kACPIStatus_Failure_ResourceNotFound;
			
			return;
			
		}
		
		uint16_t Value;
		uint32_t EventBlockSize = FADT :: GetPM1EventBlockByteCount ( & SubStatus );
		
		if ( SubStatus != kACPIStatus_Success )
		{
			
			* Status = kACPIStatus_Failure_ResourceNotFound;
			
			return;
			
		}
		
		// We're at a point now where everything /should/ succeed, so add the APCI event hook.
		
		InitSCIHandlerHook ( & TimerIntHook, & Interrupt );
		AddSCIHandlerHook ( & TimerIntHook );
		
		// Now enable the timer
		
		if ( ( PM1aEventAddress.Address != 0 ) && ( ! FailA ) )
		{
			
			switch ( PM1aEventAddress.AddressSpaceID )
			{
				
			case kACPIAddress_AddressSpaceID_Memory:
				
				Value = * reinterpret_cast <uint16_t *> ( PM1aEventAddress.Address + ( EventBlockSize / 2 ) );
				Value |= kACPIFixedRegister_PM1Enable_Flag_PMTimer;
				* reinterpret_cast <uint16_t *> ( PM1aEventAddress.Address + EventBlockSize / 2 ) = Value;
				
				EventAddressA = PM1aEventAddress.Address;
				EventAddressSpaceA = kACPIAddress_AddressSpaceID_Memory;
				
				break;
				
			case kACPIAddress_AddressSpaceID_SystemIO:
				
				Value = HW::CPU::IO :: In16 ( PM1aEventAddress.Address + ( EventBlockSize / 2 ) );
				Value |= kACPIFixedRegister_PM1Enable_Flag_PMTimer;
				HW::CPU::IO :: Out16 ( PM1aEventAddress.Address + ( EventBlockSize / 2 ), Value );
				
				EventAddressA = PM1aEventAddress.Address;
				EventAddressSpaceA = kACPIAddress_AddressSpaceID_SystemIO;
				
				break;
				
			default:
				* Status = kACPIStatus_Failure_UnsupportedAddressSpace;
				return;
				
			}
			
		}
		
		if ( PM1bEventAddress.Address != 0 )
		{
			
			switch ( PM1bEventAddress.AddressSpaceID )
			{
				
			case kACPIAddress_AddressSpaceID_Memory:
				
				Value = * reinterpret_cast <uint16_t *> ( PM1bEventAddress.Address + ( EventBlockSize / 2 ) );
				Value &= ~ kACPIFixedRegister_PM1Enable_Flag_PMTimer;
				* reinterpret_cast <uint16_t *> ( PM1bEventAddress.Address + ( EventBlockSize / 2 ) ) = Value;
				
				EventAddressB = PM1bEventAddress.Address;
				EventAddressSpaceB = kACPIAddress_AddressSpaceID_Memory;
				
				break;
				
			case kACPIAddress_AddressSpaceID_SystemIO:
				
				Value = HW::CPU::IO :: In16 ( PM1bEventAddress.Address + ( EventBlockSize / 2 ) );
				Value |= kACPIFixedRegister_PM1Enable_Flag_PMTimer;
				HW::CPU::IO :: Out16 ( PM1bEventAddress.Address + ( EventBlockSize / 2 ), Value );
				
				EventAddressB = PM1bEventAddress.Address;
				EventAddressSpaceB = kACPIAddress_AddressSpaceID_SystemIO;
				
				break;
				
			default:
				* Status = kACPIStatus_Failure_UnsupportedAddressSpace;
				return;
				
			}
			
		}
		
		Exist = true;
		
	}
	else
		* Status = kACPIStatus_Failure_InvalidTable;
	
};

bool HW::ACPI::PMTimer :: Exists ()
{
	
	return Exist;
	
};

uint64_t HW::ACPI::PMTimer :: GetTimeNS ()
{
	
	if ( ! Exist )
		return 0;
	
	return ( ( TimerUpdate () * 1000000000 ) / 3579545 );
	
};

// ACPI Event handler. First checks if this event was a PMTimer event, and if so, handle it!

bool HW::ACPI::PMTimer :: Interrupt ()
{
	
	uint16_t EventValue = 0;
	
	// Check if TMR_STS was set. If so, this was a PM Timer event.
	
	if ( EventAddressA != 0 )
	{
		
		switch ( EventAddressSpaceA )
		{
		
		case kACPIAddress_AddressSpaceID_Memory:
			EventValue |= * reinterpret_cast <volatile uint16_t *> ( EventAddressA );
			break;
			
		case kACPIAddress_AddressSpaceID_SystemIO:
			EventValue |= HW::CPU::IO :: In16 ( EventAddressA );
			break;
			
		}
		
	}
	
	if ( EventAddressB != 0 )
	{
		
		switch ( EventAddressSpaceB )
		{
		
		case kACPIAddress_AddressSpaceID_Memory:
			EventValue |= * reinterpret_cast <volatile uint16_t *> ( EventAddressB );
			break;
			
		case kACPIAddress_AddressSpaceID_SystemIO:
			EventValue |= HW::CPU::IO :: In16 ( EventAddressB );
			break;
			
		}
		
	}
	
	// This was a PM Timer event. Handle it, then reset the status register.
	
	if ( EventValue & FADT :: kRegister_PM1Event_Flag_PMTimerStatus )
	{
		
		TimerUpdate ();
		
		if ( EventAddressA != 0 )
		{
			
			switch ( EventAddressSpaceA )
			{
			
			case kACPIAddress_AddressSpaceID_Memory:
				* reinterpret_cast <volatile uint16_t *> ( EventAddressA ) = EventValue;
				break;
				
			case kACPIAddress_AddressSpaceID_SystemIO:
				HW::CPU::IO :: Out16 ( EventAddressA, EventValue );
				break;
				
			}
			
		}
		
		if ( EventAddressB != 0 )
		{
			
			switch ( EventAddressSpaceB )
			{
			
			case kACPIAddress_AddressSpaceID_Memory:
				* reinterpret_cast <volatile uint16_t *> ( EventAddressB ) = EventValue;
				break;
				
			case kACPIAddress_AddressSpaceID_SystemIO:
				HW::CPU::IO :: Out16 ( EventAddressB, EventValue );
				break;
				
			}
			
		}
		
	}
	
	return false;
	
};

// Updates the grand timer tick value. This is the actual timer read.

uint64_t HW::ACPI::PMTimer :: TimerUpdate ()
{
	
	uint64_t Return;
	
	bool ReInt = Interrupt::IState :: ReadAndSetBlock ();
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	uint32_t Current;
	
	switch ( TimerAddressSpace )
	{
	
	case kACPIAddress_AddressSpaceID_Memory:
		Current = * reinterpret_cast <volatile uint32_t *> ( TimerAddress ) & RegisterMask;
		break;
		
	case kACPIAddress_AddressSpaceID_SystemIO:
		Current = HW::CPU::IO :: In32 ( TimerAddress ) & RegisterMask;
		break;
		
	default:
		Current = 0;
		break;
		
	}
	
	Count += static_cast <uint64_t> ( ( Current - LastTValue ) & RegisterMask );
	LastTValue = Current;
	
	Return = Count;
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	Interrupt::IState :: WriteBlock ( ReInt );
	
	return Return;
	
};
