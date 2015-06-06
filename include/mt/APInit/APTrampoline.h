#ifndef MT_APINIT_APTRAPOLINE_H
#define MT_APINIT_APTRAPOLINE_H

#include <mt/apinit/APInit.h>

#include <stdint.h>

namespace MT
{
	
	namespace APInit
	{
		
		class APTrampoline
		{
		public:
			
			static void Init ();
			static void SetStack ( void * Stack );
			
			static void * GetEntry ();
			
		private:
			
			typedef struct
			{
				
				uint32_t PModeMainPTR;
				uint32_t InitStackTop;
				
				uint32_t Trampoline;
				
			} __attribute__ (( packed )) TrapolineBinary;
			
			static TrapolineBinary * Binary;
			
		};
		
	};
	
};

#endif
