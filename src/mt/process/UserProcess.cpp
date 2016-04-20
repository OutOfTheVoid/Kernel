#include <system/userspace/UserProcess.h>

Synchronization::Spinlock :: Spinlock_t System::UserSpace::UserProcess :: ProcessIDLock = Synchronization::Spinlock :: Initializer ();
uint64_t System::UserSpace::UserProcess :: MaxProcessID = 0;

void System::UserSpace::UserProcess :: Init ()
{
};

uint64_t System::UserSpace::UserProcess :: GetNewProcessID ()
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & ProcessIDLock );
	
	uint64_t ID = MaxProcessID;
	MaxProcessID ++;
	
	MT::Synchronization::Spinlock :: Release ( & ProcessIDLock );
	
	return ID;
	
};
