#ifndef MT_SYNCHRONIZATION_H
#define MT_SYNCHRONIZATION_H

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
				
				Tasking::Task :: Task_t * Owner;
				Tasking::Task :: Task_t * LastWaiter;
				
			} Mutex_t;
			
			static inline Mutex_t Initializer ()
			{
				
				Mutex_t New;
				
				New.MLock = Spinlock :: Initializer ();
				
				New.Locked = false;
				
				New.Owner = NULL;
				New.LastWaiter = NULL;
				
				return New;
				
			};
			
			static void Acquire ( Mutex_t * Lock );
			static bool TryAcquire ( Mutex_t * Lock );
			
			static void Release ( Mutex_t * Lock );
			
		private:
			
		};
		
	};
	
};

#endif
