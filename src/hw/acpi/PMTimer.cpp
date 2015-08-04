#include <hw/acpi/PMTimer.h>
#include <hw/acpi/FADT.h>

bool HW::ACPI::PMTimer :: Exists = false;

bool HW::ACPI::PMTimer :: RegisterLength = kRegisterLength_24;

uint64_t HW::ACPI::PMTimer :: IntCount = 0;

//MT::Synchronization::Spinlock :: Spinlock_t HW::ACPI::PMTimer :: Lock = MT::Synchronization::Spinlock :: Initializer;

bool HW::ACPI::PMTimer :: IHandlerCompleted = false;

void HW::ACPI::PMTimer :: Init ()
{
	
	if ( ! HW::ACPI::FADT :: Valid () )
		return;
	
	
	
};

bool HW::ACPI::PMTimer :: Exists ()
{
	
	return Exists;
	
};

uint64_t HW::ACPI::PMTimer :: GetTimeNS ()
{
	
	if ( ! Exists )
		return 0;
	
	
	
};

void HW::ACPI::PMTimer :: TimerEvent ()
{
	
	
	
};
