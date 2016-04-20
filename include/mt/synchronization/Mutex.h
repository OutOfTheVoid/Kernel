#ifndef MT_SYNCHRONIZATION_MUTEX_H
#define MT_SYNCHRONIZATION_MUTEX_H

#include <stdint.h>

#include <mt/synchronization/Synchronization.h>
#include <mt/synchronization/Spinlock.h>

#include <mt/tasking/Task.h>

namespace MT
{
	
	namespace Synchronization
	{
		
		class Mutex
		{
		public:
			
			typedef struct
			{
				
				Spinlock :: Spinlock_t MLock;
				
				bool Locked;
				
				volatile Tasking :: Task * Owner;
				volatile Tasking :: Task * FirstWaiter;
				volatile Tasking :: Task * LastWaiter;
				
			} Mutex_t;
			
			static inline Mutex_t Initializer ()
			{
				
				Mutex_t New;
				
				New.MLock = Spinlock :: Initializer ();
				
				New.Locked = false;
				
				New.Owner = NULL;
				New.FirstWaiter = NULL;
				New.LastWaiter = NULL;
				
				return New;
				
			};
			
			static void Acquire ( Mutex_t * Lock );
			static bool TryAcquire ( Mutex_t * Lock );
			
			static void Release ( Mutex_t * Lock );
			
			static void ReleaseFromWrongThread ( Mutex_t * Lock );
			
		};
		
	};
	
};

#endif
