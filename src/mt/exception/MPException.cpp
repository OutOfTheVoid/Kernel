#include <MT/Exception/MPException.h>

#include <Interrupt/APIC.h>

bool MT::Exception::MPException :: Triggered = false;

void MT::Exception::MPException :: Init ()
{
	
	Triggered = false;
	
};

void MT::Exception::MPException :: Trigger ()
{
	
	Triggered = true;
	
	Interrupt::APIC :: SendBroadNMI ();
	
};

bool MT::Exception::MPException :: HasTriggered ()
{
	
	return Triggered;
	
};
