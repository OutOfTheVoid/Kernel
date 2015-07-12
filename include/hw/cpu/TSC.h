#include <hw/cpu/CPU.h>

#include <stdint.h>

namespace HW
{
	
	namespace CPU
	{
		
		class TSC
		{
		public:
			
			static inline uint64_t Read ()
			{
				
				uint64_t Return;
				
				__asm__ volatile ( "rdtsc" : "=A" ( Return ) );
				
				return Return;
				
			};
			
		};
		
	};
	
};
