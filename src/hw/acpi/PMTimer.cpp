#include <hw/acpi/PMTimer.h>
#include <hw/acpi/FADT.h>

#include <hw/cpu/IO.h>

#include <system/func/kprintf.h>

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
	
	if ( FADT :: Valid () )
	{
		
		RegisterMask = ( ( FADT :: GetFixedFeatureFlags ( & SubStatus ) & FADT :: kFixedFeatureFlag_PMTimerExtended ) != 0 ) ? kRegisterMask_32 : kRegisterMask_24;
		
		if ( SubStatus != kACPIStatus_Success )
		{
			
			* Status = kACPIStatus_Failure_ResourceNotFound;
			
			return;
			
		}
		
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
		
		ACPIAddress PM1aEventAddress = FADT :: GetPM1aEventBlockAddress ( & SubStatus );
		
		FailA = ( ( PM1aEventAddress.Address == 0 ) || ( SubStatus != kACPIStatus_Success ) );
		
		ACPIAddress PM1bEventAddress = FADT :: GetPM1bEventBlockAddress ( & SubStatus );
		
		if ( ( ( PM1bEventAddress.Address == 0 ) || ( SubStatus != kACPIStatus_Success ) ) && FailA )
		{
			
			* Status = kACPIStatus_Failure_ResourceNotFound;
			
			return;
			
		}
		
		InitSCIHandlerHook ( & TimerIntHook, & Interrupt );
		AddSCIHandlerHook ( & TimerIntHook );
		
		uint16_t Value;
		uint32_t EventBlockSize = FADT :: GetPM1EventBlockByteCount ( & SubStatus );
		
		if ( SubStatus != kACPIStatus_Success )
		{
			
			* Status = kACPIStatus_Failure_ResourceNotFound;
			
			return;
			
		}
		
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

bool HW::ACPI::PMTimer :: Interrupt ()
{
	
	uint16_t EventValue = 0;
	
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
	
	if ( EventValue & FADT :: kRegister_PM1Event_Flag_PMTimerStatus )
	{
		
		TimerUpdate ();
		
		system_func_kprintf ( "PMTimer SCI!\n" );
		
		EventValue &= ~ FADT :: kRegister_PM1Event_Flag_PMTimerStatus;
		
		if ( EventAddressA != 0 )
		{
			
			system_func_kprintf ( "Clear A!\n" );
			
			switch ( EventAddressSpaceA )
			{
			
			case kACPIAddress_AddressSpaceID_Memory:
				* reinterpret_cast <volatile uint16_t *> ( EventAddressA ) = EventValue;
				break;
				
			case kACPIAddress_AddressSpaceID_SystemIO:
				HW::CPU::IO :: Out16 ( EventAddressA, EventValue );
				break;
				
			default:
				system_func_kprintf ( "Failed!\n" );
				break;
				
			}
			
		}
		
		if ( EventAddressB != 0 )
		{
			
			system_func_kprintf ( "Clear B!\n" );
			
			switch ( EventAddressSpaceB )
			{
			
			case kACPIAddress_AddressSpaceID_Memory:
				* reinterpret_cast <volatile uint16_t *> ( EventAddressB ) = EventValue;
				break;
				
			case kACPIAddress_AddressSpaceID_SystemIO:
				HW::CPU::IO :: Out16 ( EventAddressB, EventValue );
				break;
				
			default:
				system_func_kprintf ( "Failed!\n" );
				break;
				
			}
			
		}
		
	}
	
	return false;
	
};

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
	
	//system_func_kprintf ( "PMTimer Value: %h\n", Current );
	
	Return = Count;
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	Interrupt::IState :: WriteBlock ( ReInt );
	
	return Return;
	
};
