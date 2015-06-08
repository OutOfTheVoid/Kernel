#ifndef HW_CPU_PROCESSOR_H
#define HW_CPU_PROCESSOR_H

#include <hw/cpu/CPU.h>

namespace HW
{
	
	namespace CPU
	{
		
		class Processor
		{
		public:
			
			static void Init ();
			
		private:
			
			typedef struct
			{
				
				
				
			} CPUInfo;
			
			static CPUInfo * CPUs;
			
		};
		
	};
	
};

#endif
