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
			
			typedef uint32_t Spinlock_t;
			
			static void SpinAcquire ( Spinlock_t * Lock );
			static bool TryAcquire ( Spinlock_t * Lock );
			
			static void Release ( Spinlock_t * Lock );
			
			static inline Spinlock_t Initializer ()
			{
				
				return static_cast <Spinlock_t> ( 0 );
				
			};
			
			static void Initialize ( Spinlock_t * Lock );
			
		};
		
	};
	
};

#endif
