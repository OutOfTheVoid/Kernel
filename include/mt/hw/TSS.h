#ifndef MT_HW_TSS_H
#define MT_HW_TSS_H

#include <stdint.h>

namespace MT
{
	
	namespace HW
	{
		
		class TSS
		{
		public:
			
			typedef struct
			{
				
				uint16_t BackLink;
				uint16_t BackLink_High;
				
				uint32_t ESP0;
				uint16_t SS0;
				uint16_t SS0_High;
				
				uint32_t ESP1;
				uint16_t SS1;
				uint16_t SS1_High;
				
				uint32_t ESP2;
				uint16_t SS2;
				uint16_t SS2_High;
				
				uint32_t CR3;
				
				uint32_t EIP;
				uint32_t EFlags;
				
				uint32_t EAX;
				uint32_t ECX;
				uint32_t EDX;
				uint32_t EBX;
				
				uint32_t ESP;
				uint32_t EBP;
				uint32_t ESI;
				uint32_t EDI;
				
				uint16_t ES;
				uint16_t ES_High;
				uint16_t CS;
				uint16_t CS_High;
				uint16_t SS;
				uint16_t SS_High;
				uint16_t DS;
				uint16_t DS_High;
				uint16_t FS;
				uint16_t FS_High;
				uint16_t GS;
				uint16_t GS_High;
				
				uint16_t LDT;
				uint16_t LDT_High;
				
				uint16_t Trace;
				
				uint16_t Bitmap;
				
			} __attribute__ (( packed )) TSS_t;
			
			static inline void Flush ( uint16_t Segment )
			{
				
				__asm__ volatile ( "ltr ax" : : "a" ( Segment ) );
				
			};
			
		};
		
	}
	
};

#endif
