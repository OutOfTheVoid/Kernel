#ifndef MT_TIMING_HPET_H
#define MT_TIMING_HPET_H

#include <mt/timing/Timing.h>

#include <util/Vector.h>

namespace MT
{
	
	namespace Timing
	{
		
		class HPET
		{
		public:	
			
			static void Init ();
			
		private:
			
			typedef struct
			{
				
				void * AddressPhysical;
				void * AddressVirtual;
				
			} HPETControl;
			
			Vector <HPETControl> * HPETs;
			
			
		};
		
	};
	
};

#endif
