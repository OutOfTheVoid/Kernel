#ifndef MT_SYCNHRONIZATION_SPINLOCK_H
#define MT_SYCNHRONIZATION_SPINLOCK_H

#include <stdint.h>

namespace MT
{
	
	namespace Synchronization
	{
		
		class Spinlock
		{
		public:
			
			typedef struct
			{
				
				uint32_t Lock;
				bool ReInt;
				
			} Spinlock_t;
			
			static void SpinAcquire ( Spinlock_t * Lock );
			static bool TryAcquire ( Spinlock_t * Lock );
			
			static void Release ( Spinlock_t * Lock );
			
			static inline Spinlock_t Initializer ()
			{
				
				Spinlock_t New;
				
				New.Lock = 0;
				New.ReInt = false;
				
				return New;
				
			};
			
			static void Initialize ( Spinlock_t * Lock );
			
		};
		
	};
	
};

#endif
