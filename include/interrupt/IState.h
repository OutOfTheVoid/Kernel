#ifndef INTERRUPT_ISTATE_H
#define INTERRUPT_ISTATE_H

#include <interrupt/Interrupt.h>

#include <stdint.h>

namespace Interrupt
{
	
	class IState
	{
	public:
		
		static void Init ();
		
		static void IncrementBlock ();
		static void DecrementBlock ();
		
	private:
		
		static uint32_t Count;
		
	};
	
};

#endif
