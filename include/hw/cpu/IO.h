#ifndef HW_CPU_IO_H
#define HW_CPU_IO_H

#include <hw/HW.h>
#include <hw/cpu/CPU.h>

namespace HW
{
	
	namespace CPU
	{
		
		class IO
		{
		public:
			
			static inline void Out8 ( uint16_t Port, uint8_t Value )
			{
				
				__asm__ volatile ( "outb %1, %0" : : "a" ( Value ), "dN" ( Port ) );
				
			};
			
			static inline void Out16 ( uint16_t Port, uint16_t Value )
			{
				
				__asm__ volatile ( "outw %1, %0" : : "a" ( Value ), "dN" ( Port ) );
				
			};
			
			static inline void Out32 ( uint16_t Port, uint32_t Value )
			{
				
				__asm__ volatile ( "out %1, %0" : : "a" (Value), "dN" (Port) );
				
			};
			
			static inline uint8_t In8 ( uint16_t Port )
			{
				
				uint8_t Ret;
				
				__asm__ volatile ( "inb %0, %1" : "=a" ( Ret ) : "dN" ( Port ) );
				
				return Ret;
				
			};
			
			static inline uint16_t In16 ( uint16_t Port )
			{
				
				uint16_t Ret;
				
				__asm__ volatile ( "inw AX, DX" : "=AX" ( Ret ) : "dN" ( Port ) );
				
				return Ret;
				
			};
			
			static inline uint32_t In32 ( uint16_t Port )
			{
				
				uint32_t Ret;
				
				__asm__ volatile ( "in EAX, DX" : "=EAX" ( Ret ) : "dN" ( Port ) );
				
				return Ret;
				
			};
			
			static inline void IOWait ()
			{
				
				Out8 ( 0x80, 0 );
				
			};
			
		};
		
	};
	
};

#endif
