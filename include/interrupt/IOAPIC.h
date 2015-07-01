#ifndef INTERRUPT_IOAPIC_H
#define INTERRUPT_IOAPIC_H

#include <interrupt/Interrupt.h>

#include <stdint.h>

namespace Interrupt
{
	
	class IOAPIC
	{
	public:	
		
		void Init ();
		
	private:
		
		uint32_t Count;
		uint32_t ** BaseAddresses;
		
	};
	
};

#endif
