#ifndef MT_TASKING_SWITCHER_H
#define MT_TASKING_SWITCHER_H

#include <mt/tasking/Tasking.h>
#include <mt/tasking/Task.h>

namespace MT
{
	
	namespace Tasking
	{
		
		class Switcher
		{
		public:
		
			static void Init ();
			static void SetupInitISRReturnStack ( Task :: Task_t * EntryTask, void * KStackBottom, uint32_t StackSize );
			
		private:
			
			static void StackPush ( void ** Stack, uint32_t Value );
			static uint32_t StackPop ( void ** Stack );
			static uint32_t StackPeek ( void ** Stack );
			
		};
		
	};
	
};

#endif
