#ifndef MT_SYNCHRONIZATION_RECURSIVESPINLOCK_H
#define MT_SYNCHRONIZATION_RECURSIVESPINLOCK_H

#include <mt/synchronization/Synchronization.h>
#include <mt/synchronization/Spinlock.h>

#include <hw/cpu/Processor.h>

#include <stdint.h>

namespace MT
{
	
	namespace Synchronization
	{
		
		class RecursiveSpinlock
		{
		public:
			
			typedef struct
			{
				
				uint32_t Lock;
				bool ReInt;
				
				uint32_t OwnderAPICID;
				
				uint32_t Count;
				
			} RecursiveSpinlock_t;
			
			static inline RecursiveSpinlock_t Initializer ()
			{
				
				RecursiveSpinlock_t New = RecursiveSpinlock_t ();
				
				New.Lock = 0;
				New.ReInt = false;
				
				New.OwnderAPICID = 0;
				New.Count = 0;
				
				return New;
				
			};
			
			static void Initialize ( RecursiveSpinlock_t * Lock );
			
			static bool TrySpinAcquire ( RecursiveSpinlock_t * Lock );
			static void SpinAcquire ( RecursiveSpinlock_t * Lock );
			static void Release ( RecursiveSpinlock_t * Lock );
			
		};
		
	};
	
};

#endif
