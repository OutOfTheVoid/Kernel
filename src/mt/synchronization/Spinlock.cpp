#include <mt/synchronization/Spinlock.h>

#include <cpputil/Linkage.h>

C_LINKAGE uint32_t mt_synchronization_tryslock ( MT::Synchronization::Spinlock :: Spinlock_t * Lock );
C_LINKAGE void mt_synchronization_pwacquirelock ( MT::Synchronization::Spinlock :: Spinlock_t * Lock );
C_LINKAGE void mt_synchronization_release ( MT::Synchronization::Spinlock :: Spinlock_t * Lock );


void MT::Synchronization::Spinlock :: SpinAcquire ( Spinlock_t * Lock )
{
	
	mt_synchronization_pwacquirelock ( Lock );
	
};

bool MT::Synchronization::Spinlock :: TryAcquire ( Spinlock_t * Lock )
{
	
	return mt_synchronization_tryslock ( Lock ) == 1;
	
};

void MT::Synchronization::Spinlock :: Release ( Spinlock_t * Lock )
{
	
	mt_synchronization_release ( Lock );
	
};

MT::Synchronization::Spinlock :: Spinlock_t MT::Synchronization::Spinlock :: Initializer ()
{
	
	return static_cast <Spinlock_t> ( 0 );
	
};

void MT::Synchronization::Spinlock :: Initialize ( Spinlock_t * Lock )
{
	
	* Lock = static_cast <Spinlock_t> ( 0 );
	
};
