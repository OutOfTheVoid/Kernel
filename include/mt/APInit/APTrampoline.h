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
			static void SetPagingDirectory ( uint32_t * PagingDirectory );
			static uint32_t GetStartupPage ();
			
		private:
			
			typedef struct
			{
				
				uint8_t Code [ 508 ];
				
				uint32_t Signature;
				uint32_t PModeMainPTR;
				uint32_t InitStackTop;
				uint32_t * PagingDirectory;
				
			} __attribute__ (( packed )) TrampolineBinary;
			
			static TrampolineBinary * Binary;
			
		};
		
	};
	
};

#endif
