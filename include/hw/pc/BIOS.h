#ifndef HW_PC_BIOS_H
#define HW_PC_BIOS_H

#include <hw/pc/PC.h>

#include <stdint.h>

namespace HW
{
	
	
	namespace PC
	{
		
		
		class BIOS
		{
		public:
			
			static void ConfigWarmBootJump ( uint32_t Address );
			
		private:
			
			static const uint32_t kAddress_WarmBootVector = 0x467;
			
		};
		
		
	};
	
	
};

#endif
