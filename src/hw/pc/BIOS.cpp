#include <hw/pc/BIOS.h>

void HW::PC::BIOS :: ConfigWarmBootJump ( uint32_t Address )
{
	
	uint16_t * ConfigWord = reinterpret_cast <uint16_t *> ( kAddress_WarmBootVector );
	
	* ConfigWord = Address >> 4;
	
};
