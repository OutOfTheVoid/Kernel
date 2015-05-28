#ifndef INTERRUPT_APIC_H
#define INTERRUPT_APIC_H

#include <stdint.h>

namespace Interrupt
{
	
	class APIC
	{
	public:
		
		static void Init ();
		static bool HasAPIC ();
		
	private:
		
		static const uint32_t kAPIC_InitialMMIOBase =  0xFEE00000;
		
		static void SetAPICBaseAddress ( uint32_t Base );
		static uint32_t GetAPICBaseAddress ();
		
		static void EnableAPIC ();
		
	};
	
};

#endif
