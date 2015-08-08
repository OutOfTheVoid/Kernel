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

uint32_t HW::ACPI::PMTimer :: AddressSpace = false;
uint32_t HW::ACPI::PMTimer :: Address = 0;

MT::Synchronization::Spinlock :: Spinlock_t HW::ACPI::PMTimer :: Lock = MT::Synchronization::Spinlock :: Initializer ();

void HW::ACPI::PMTimer :: Init ( uint32_t * Status )
{
	
	uint32_t SubStatus;
	
	bool FailA;
	
	if ( FADT :: Valid () )
	{
		
		FADT :: WriteACPIEnable ( true, & SubStatus );
		
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
				
				Value = * reinterpret_cast <uint16_t *> ( PM1aEventAddress.Address + EventBlockSize / 2 );
				Value |= kACPIFixedRegister_PM1Enable_Flag_PMTimer;
				* reinterpret_cast <uint16_t *> ( PM1aEventAddress.Address + EventBlockSize / 2 ) = Value;
				
				break;
				
			case kACPIAddress_AddressSpaceID_SystemIO:
				
				Value = HW::CPU::IO :: In16 ( PM1aEventAddress.Address + EventBlockSize / 2 );
				Value |= kACPIFixedRegister_PM1Enable_Flag_PMTimer;
				HW::CPU::IO :: Out16 ( PM1aEventAddress.Address + EventBlockSize / 2, Value );
				
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
				
				Value = * reinterpret_cast <uint16_t *> ( PM1bEventAddress.Address + EventBlockSize / 2 );
				Value |= kACPIFixedRegister_PM1Enable_Flag_PMTimer;
				* reinterpret_cast <uint16_t *> ( PM1bEventAddress.Address + EventBlockSize / 2 ) = Value;
				
				break;
				
			case kACPIAddress_AddressSpaceID_SystemIO:
				
				Value = HW::CPU::IO :: In16 ( PM1bEventAddress.Address + EventBlockSize / 2 );
				Value |= kACPIFixedRegister_PM1Enable_Flag_PMTimer;
				HW::CPU::IO :: Out16 ( PM1bEventAddress.Address + EventBlockSize / 2, Value );
				
				break;
				
			default:
				* Status = kACPIStatus_Failure_UnsupportedAddressSpace;
				return;
				
			}
			
		}
		
	}
	else
		* Status = kACPIStatus_Failure_InvalidTable;
	
	if ( * Status == kACPIStatus_Success )
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
				
				void * TimerBlockAddressVirtual = mm_kvmap ( reinterpret_cast <void *> ( TimerBlockAddress.Address ), 0x1000, MM::Paging::PageTable :: Flags_NoCache );
				
				if ( TimerBlockAddressVirtual == NULL )
				{
					
					* Status = kACPIStatus_Failure_System_OutOfMemory;
					
					return;
					
				}
				
				Address = reinterpret_cast <uint32_t> ( TimerBlockAddressVirtual );
				AddressSpace = kACPIAddress_AddressSpaceID_Memory;
				
				LastTValue = ( * reinterpret_cast <uint32_t *> ( TimerBlockAddressVirtual ) ) & RegisterMask;
				
			}
			break;
			
		case kACPIAddress_AddressSpaceID_SystemIO:
			{
				
				Address = TimerBlockAddress.Address;
				AddressSpace = kACPIAddress_AddressSpaceID_SystemIO;
				
				LastTValue = HW::CPU::IO :: In32 ( Address ) & RegisterMask;
				
			}
			break;
			
		default:
			
			* Status = kACPIStatus_Failure_UnsupportedAddressSpace;
			
			return;
			
			break;
			
		}
		
		Exist = true;
		
	}
	
};

bool HW::ACPI::PMTimer :: Exists ()
{
	
	return Exist;
	
};

uint64_t HW::ACPI::PMTimer :: GetTimeNS ()
{
	
	if ( ! Exist )
		return 0;
	
	return TimerUpdate ();
	
};

uint64_t HW::ACPI::PMTimer :: TimerUpdate ()
{
	
	uint64_t Return;
	
	bool ReInt = Interrupt::IState :: ReadAndSetBlock ();
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	uint32_t Current;
	
	switch ( AddressSpace )
	{
	
	case kACPIAddress_AddressSpaceID_Memory:
		Current = * reinterpret_cast <uint32_t *> ( Address ) & RegisterMask;
		break;
		
	case kACPIAddress_AddressSpaceID_SystemIO:
		Current = HW::CPU::IO :: In32 ( Address ) & RegisterMask;
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
