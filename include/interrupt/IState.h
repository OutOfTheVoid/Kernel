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
		
		static uint32_t GetBlockCount ();
		
		static bool ReadAndSetBlock ();
		static void WriteBlock ( bool UnBlock );
		
	private:
		
		static uint32_t BSPCount;
		
	};
	
};

#endif
