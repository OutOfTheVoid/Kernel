#ifndef HW_CPU_ATOMIC_H
#define HW_CPU_ATOMIC_H

namespace HW
{
	
	namespace CPU
	{
		
		class Atomic
		{
		public:
			
			static inline void Write ( volatile uint32_t * Variable, uint32_t Value )
			{
				
				* Variable = Value;
				
			};
			
			static inline uint32_t Read ( volatile uint32_t * Variable )
			{
				
				return * Variable;
				
			};
			
			static inline bool IncrementTestOne ( volatile uint32_t * Variable )
			{
				
				uint32_t Value = 1;
				
				__asm__ volatile ( "lock xadd [ %2 ], eax" : "=a" ( Value ) : "a" ( Value ), "m" ( * Variable ) );
				
				return ( Value == 0 );
				
			};
			
			static inline void Increment ( volatile uint32_t * Variable )
			{
				
				__asm__ volatile ( "lock inc [ %0 ]" :: "m" ( * Variable ) );
				
			};
			
			static inline bool DecrementTestZero ( volatile uint32_t * Variable )
			{
				
				volatile uint32_t Zero = 0;
				
				__asm__ volatile ( "lock dec [ %0 ]\nmov [ %2 ], 0\nsetnz [ %3 ]" : "=m" ( Variable ) : "m" ( Variable ), "m" ( Zero ), "r" ( Zero ) : "memory" );
				
				return ( Zero == 1 );
				
			};
			
		};
		
	};
	
};

#endif
