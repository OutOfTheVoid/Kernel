#include <mt/synchronization/RecursiveSpinlock.h>

#include <interrupt/IState.h>

#include <cpputil/Linkage.h>

#include <hw/cpu/Processor.h>

ASM_LINKAGE uint32_t mt_synchronization_tryslock ( uint32_t * Lock );
ASM_LINKAGE void mt_synchronization_pwacquirelock ( uint32_t * Lock );
ASM_LINKAGE void mt_synchronization_release ( uint32_t * Lock );

void MT::Synchronization::RecursiveSpinlock :: Initialize ( RecursiveSpinlock_t * Lock )
{
	
	Lock -> Lock = 0;
	Lock -> ReInt = false;
	
	Lock -> OwnderAPICID = 0;
	Lock -> Count = 0;
	
};

bool MT::Synchronization::RecursiveSpinlock :: TrySpinAcquire ( RecursiveSpinlock_t * Lock )
{
	
	bool ReInt = Interrupt::IState :: ReadAndSetBlock ();
	
	mt_synchronization_pwacquirelock ( & Lock -> Lock );

	uint32_t APICID = ::HW::CPU::Processor :: GetCurrentAPICID32 ();

	if ( Lock -> Count == 0 )
	{

		Lock -> Count = 1;
		Lock -> OwnderAPICID = APICID;

		Lock -> ReInt = ReInt;

		mt_synchronization_release ( & Lock -> Lock );

		return true;

	}

	if ( Lock -> OwnderAPICID == APICID )
	{

		Lock -> Count ++;

		mt_synchronization_release ( & Lock -> Lock );

		return true;

	}

	mt_synchronization_release ( & Lock -> Lock );
	Interrupt::IState :: WriteBlock ( ReInt );

	return false;
	
};

void MT::Synchronization::RecursiveSpinlock :: SpinAcquire ( RecursiveSpinlock_t * Lock )
{
	
	while ( ! TrySpinAcquire ( Lock ) );
	
};

void MT::Synchronization::RecursiveSpinlock :: Release ( RecursiveSpinlock_t * Lock )
{

	Interrupt::IState :: ReadAndSetBlock ();
	
	mt_synchronization_pwacquirelock ( & Lock -> Lock );
	
	if ( Lock -> Count > 0 )
		Lock -> Count --;

	if ( Lock -> Count == 0 )
	{

		bool ReInt = Lock -> ReInt;

		mt_synchronization_release ( & Lock -> Lock );

		Interrupt::IState :: WriteBlock ( ReInt );

		return;

	}
	
	mt_synchronization_release ( & Lock -> Lock );
	
};
