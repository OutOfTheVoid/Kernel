#include <mt/synchronization/Spinlock.h>

#include <interrupt/IState.h>

#include <cpputil/Linkage.h>

ASM_LINKAGE uint32_t mt_synchronization_tryslock ( uint32_t * Lock );
ASM_LINKAGE void mt_synchronization_pwacquirelock ( uint32_t * Lock );
ASM_LINKAGE void mt_synchronization_release ( uint32_t * Lock );


void MT::Synchronization::Spinlock :: SpinAcquire ( Spinlock_t * Lock )
{
	
	Lock -> ReInt = Interrupt::IState :: ReadAndSetBlock ();
	mt_synchronization_pwacquirelock ( & Lock -> Lock );
	
};

bool MT::Synchronization::Spinlock :: TryAcquire ( Spinlock_t * Lock )
{
	bool ReInt = Interrupt::IState :: ReadAndSetBlock ();
	
	if ( mt_synchronization_tryslock ( & Lock -> Lock ) == 1 )
	{
		
		Interrupt::IState :: WriteBlock ( ReInt );
		
		return false;
		
	}
	
	Lock -> ReInt = ReInt;
	
	return true;
	
};

void MT::Synchronization::Spinlock :: Release ( Spinlock_t * Lock )
{
	
	mt_synchronization_release ( & Lock -> Lock );
	Interrupt::IState :: WriteBlock ( Lock -> ReInt );
	
};

void MT::Synchronization::Spinlock :: Initialize ( Spinlock_t * Lock )
{
	
	Lock -> Lock = 0;
	Lock -> ReInt = false;
	
};
