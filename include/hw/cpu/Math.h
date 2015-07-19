#ifndef HW_CPU_MATH_H
#define HW_CPU_MATH_H

#include <hw/cpu/CPU.h>

#include <stdint.h>

#define HW_CPU_MATH_CR0_TS 0x00000008

namespace HW
{
	
	namespace CPU
	{
		
		class Math
		{
		public:
			
			typedef struct
			{
				
				uint16_t FP_Control;
				uint16_t FP_Status;
				uint16_t FP_Tag;
				
				uint16_t FP_Opcode;
				
				uint32_t FP_InstructionPointer;
				uint32_t FP_CS;
				
				uint32_t FP_DataPointer;
				uint32_t FP_DS;
				
				uint32_t MXCSR;
				
				uint32_t Unused;
				
				uint64_t ST0_MM0_Low;
				uint64_t ST0_MM0_High;
				
				uint64_t ST1_MM1_Low;
				uint64_t ST1_MM1_High;
				
				uint64_t ST2_MM2_Low;
				uint64_t ST2_MM2_High;
				
				uint64_t ST3_MM3_Low;
				uint64_t ST3_MM3_High;
				
				uint64_t ST4_MM4_Low;
				uint64_t ST4_MM4_High;
				
				uint64_t ST5_MM5_Low;
				uint64_t ST5_MM5_High;
				
				uint64_t ST6_MM6_Low;
				uint64_t ST6_MM6_High;
				
				uint64_t ST0_MM7_Low;
				uint64_t ST0_MM7_High;
				
				uint64_t XMM0_Low;
				uint64_t XMM0_High;
				
				uint64_t XMM1_Low;
				uint64_t XMM1_High;
				
				uint64_t XMM2_Low;
				uint64_t XMM2_High;
				
				uint64_t XMM3_Low;
				uint64_t XMM3_High;
				
				uint64_t XMM4_Low;
				uint64_t XMM4_High;
				
				uint64_t XMM5_Low;
				uint64_t XMM5_High;
				
				uint64_t XMM6_Low;
				uint64_t XMM6_High;
				
				uint64_t XMM7_Low;
				uint64_t XMM7_High;
				
				uint64_t XMM8_Low;
				uint64_t XMM8_High;
				
				char Reserved [ 224 ];
				
			} __attribute__ (( packed )) MathState;
			
			static inline void SaveState ( MathState * State )
			{
				
				__asm__ volatile ( "fxsave [ %0 ]" : : "r" ( State ) );
				
			};
			
			static inline void LoadState ( MathState * State )
			{
				
				__asm__ volatile ( "fxrstor [ %0 ]" : : "r" ( State ) );
				
			};
			
			static inline void InitState ()
			{
				
				__asm__ volatile ( "fninit" );
				
			};
			
			static inline void ClearTest ()
			{
				
				uint32_t CR0;
				
				__asm__ volatile ( "mov %0, cr0" : "=r" ( CR0 ) );
				CR0 &= ~ HW_CPU_MATH_CR0_TS;
				__asm__ volatile ( "mov cr0, %0" : : "r" ( CR0 ) );
				
			};
			
			static inline bool GetTest ()
			{
				
				uint32_t CR0 = 0;
	
	__asm__ volatile ( "mov %0, cr0" : : "r" ( CR0 ) );
	
	return ( CR0 & HW_CPU_MATH_CR0_TS ) != 0;
				
			};
			
		};
		
	};
	
};

#endif
