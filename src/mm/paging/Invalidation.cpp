#include <mm/paging/Invalidation.h>

#include <interrupt/APIC.h>

void MM::Paging::Invalidation :: PInit ()
{
	
	
	
};

void MM::Paging::Invalidation :: InvalidatePage ( uint32_t CR3, uint32_t PageAddress )
{
	
	
	
};

bool MM::Paging::Invalidation :: PageFault ( uint32_t ErrorCode )
{
	
	uint32_t Address = GetCR2 ();
	
	
	
};
