#ifndef MT_TIMING_PIT_H
#define MT_TIMING_PIT_H

#include <mt/timing/Timing.h>

#include <stdint.h>

namespace MT
{
	
	namespace Timing
	{
		
		class PIT
		{
		public:
			
			static void Init ();
			
			
		private:
			
			static const uint32_t kDataPort_Channel0 = 0x40;
			static const uint32_t kDataPort_Channel1 = 0x41;
			static const uint32_t kDataPort_Channel2 = 0x42;
			
			static const uint32_t kCommandPort = 0x43;
			
		};
		
	};
	
};

#endif
