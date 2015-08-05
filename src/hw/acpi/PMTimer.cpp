#include <hw/acpi/PMTimer.h>
#include <hw/acpi/FADT.h>

bool HW::ACPI::PMTimer :: Exist = false;

bool HW::ACPI::PMTimer :: RegisterLength = kRegisterLength_24;

uint64_t HW::ACPI::PMTimer :: IntCount = 0;

//MT::Synchronization::Spinlock :: Spinlock_t HW::ACPI::PMTimer :: Lock = MT::Synchronization::Spinlock :: Initializer;

bool HW::ACPI::PMTimer :: IHandlerCompleted = false;

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
		
		ACPIAddress PM1a = FADT :: GetPM1aControlBlockAddress ( & SubStatus );
		
		FailA = ( ( PM1a.Address == 0 ) || ( SubStatus != kACPIStatus_Success ) );
		
		ACPIAddress PM1b = FADT :: GetPM1bControlBlockAddress ( & SubStatus );
		
		if ( ( ( PM1b.Address == 0 ) || ( SubStatus != kACPIStatus_Success ) ) && FailA )
		{
			
			* Status = kACPIStatus_Failure_ResourceNotFound;
			
			return;
			
		}
		
		uint32_t Value;
		
		if ( ( PM1a.Address != 0 ) && ( ! FailA ) )
		{
			
			switch ( PM1a.AddressSpaceID )
			{
				
			case kACPIAddress_AddressSpaceID_Memory:
				
				Value = * reinterpret_cast <uint32_t *> ( PM1a.Address );
				
				// Modify;
				
				* reinterpret_cast <uint32_t *> ( PM1a.Address ) = Value;
				
				break;
				
			case kACPIAddress_AddressSpaceID_SystemIO:
			
				break;
				
			default:
				* Status = kACPIStatus_Failure_UnsupportedAddressSpace;
				return;
				
			}
			
		}
		
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
	
	return 0;
	
};

void HW::ACPI::PMTimer :: TimerEvent ()
{
	
	
	
};
