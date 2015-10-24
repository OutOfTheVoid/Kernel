#ifndef MT_EXCEPTION_MPEXCEPTION_H
#define MT_EXCEPTION_MPEXCEPTION_H

#include <mt/exception/Exception.h>

#include <Interrupt/InterruptHandlers.h>

namespace MT
{
	
	namespace Exception
	{

		class MPException
		{
		public:
			
			static void Init ();
			
			static void Trigger ();
			static bool HasTriggered ();
			
		private:
			
			static bool Triggered;
			
		};

	};

};

#endif
