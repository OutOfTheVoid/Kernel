#ifndef MT_SYNCHRONIZATION_RWLOCK_H
#define MT_SYNCHRONIZATION_RWLOCK_H

#include <mt/synchronization/RWLock.h>
#include <mt/synchronization/Mutex.h>

namespace MT
{
	
	namespace Synchronization
	{
		
		class RWLock
		{
		public:
			
			typedef struct
			{
				
				Mutex :: Mutex_t WLock;
				
				uint32_t ReadersAtomic;
				
			} RWLock_t;
			
			static inline RWLock_t Initializer ()
			{
				
				RWLock_t New;
				
				New.WLock = Mutex :: Initializer ();
				
				New.ReadersAtomic = 0;
				
				return New;
				
			};
			
			static void ReadAcquire ( RWLock_t * Lock );
			static void ReadRelease ( RWLock_t * Lock );
			
			static void WriteAcquire ( RWLock_t * Lock );
			static void WriteRelease ( RWLock_t * Lock );
			
		};
		
	};
	
};

#endif
