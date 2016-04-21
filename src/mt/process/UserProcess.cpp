#include <mt/process/UserProcess.h>

#include <util/string/string.h>

MT::Synchronization::Spinlock :: Spinlock_t MT::Process::UserProcess :: ProcessIDLock = Synchronization::Spinlock :: Initializer ();
uint64_t MT::Process::UserProcess :: MaxProcessID = 0;

void MT::Process::UserProcess :: Init ()
{
};

MT::Process :: UserProcess * MT::Process::UserProcess :: CreateUserProcess ( const char * Name, MM::Paging::PageTable * MemoryMapping, MM::Paging::AddressSpace * MemorySpace, UserProcess * Parent )
{
	
	UserProcess * New = new UserProcess ();
	
	if ( New == NULL )
		return NULL;
	
	if ( strlen ( Name ) > 255 )
	{
		
		memcpy ( reinterpret_cast <void *> ( New -> Name ), reinterpret_cast <const void *> ( Name ), 255 );
		New -> Name [ 255 ] = 0;
		
	}
	else
		strcpy ( New -> Name, Name );
	
	New -> ProcessID = GetNewProcessID ();
	New -> Parent = Parent;
	
	New -> MemoryMapping = MemoryMapping;
	New -> MemorySpace = MemorySpace;
	
	New -> Threads = new Vector <volatile Tasking :: Task *> ();
	New -> ThreadListLock = MT::Synchronization::Spinlock :: Initializer ();
	
	return New;
	
};

uint64_t MT::Process::UserProcess :: GetNewProcessID ()
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & ProcessIDLock );
	
	uint64_t ID = MaxProcessID;
	MaxProcessID ++;
	
	MT::Synchronization::Spinlock :: Release ( & ProcessIDLock );
	
	return ID;
	
};
