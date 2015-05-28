#ifndef HW_CPU_CPUID_H
#define HW_CPU_CPUID_H

#include <hw/HW.h>
#include <hw/cpu/CPU.h>

#include <stdint.h>

namespace HW
{
	
	namespace CPU
	{
		
		class CPUID
		{
		public:
			
			const static uint32_t kRegister_EAX = 0;
			const static uint32_t kRegister_EBX = 1;
			const static uint32_t kRegister_ECX = 2;
			const static uint32_t kRegister_EDX = 3;
			
			const static uint32_t kDFlag_FPU = 0x00000001;
			const static uint32_t kDFlag_VME = 0x00000002;
			const static uint32_t kDFlag_DE = 0x00000004;
			const static uint32_t kDFlag_PSE = 0x00000008;
			const static uint32_t kDFlag_TSC = 0x00000010;
			const static uint32_t kDFlag_MSR = 0x00000020;
			const static uint32_t kDFlag_PAE = 0x00000040;
			const static uint32_t kDFlag_MCE = 0x00000080;
			const static uint32_t kDFlag_CXCHG8 = 0x00000100;
			const static uint32_t kDFlag_APIC = 0x00000200;
			const static uint32_t kDFlag_SEP = 0x00000800;
			const static uint32_t kDFlag_MTRR = 0x00001000;
			const static uint32_t kDFlag_PGE = 0x00002000;
			const static uint32_t kDFlag_MCA = 0x00004000;
			const static uint32_t kDFlag_CMOV = 0x00008000;
			const static uint32_t kDFlag_PAT = 0x00010000;
			const static uint32_t kDFlag_PSE36 = 0x00020000;
			const static uint32_t kDFlag_PSN = 0x00040000;
			const static uint32_t kDFlag_CLFL = 0x00080000;
			const static uint32_t kDFlag_DTES = 0x00200000;
			const static uint32_t kDFlag_ACPI = 0x00400000;
			const static uint32_t kDFlag_MMX = 0x00800000;
			const static uint32_t kDFlag_FXSR = 0x01000000;
			const static uint32_t kDFlag_SSE = 0x02000000;
			const static uint32_t kDFlag_SSE2 = 0x04000000;
			const static uint32_t kDFlag_SS = 0x08000000;
			const static uint32_t kDFlag_HTT = 0x10000000;
			const static uint32_t kDFlag_TM1 = 0x20000000;
			const static uint32_t kDFlag_IA64 = 0x40000000;
			const static uint32_t kDFlag_PBE = 0x80000000;
			
			static inline void GetCPUID ( int32_t Code, uint32_t Output [ 4 ] )
			{
				
				__asm__ volatile ( "cpuid" : "=a" ( Output [ 0 ] ), "=b" ( Output [ 1 ] ), "=c" ( Output [ 2 ] ), "=d" ( Output [ 3 ] ) : "a" ( Code ) );
				
			};
			
			static inline void GetCPUVendorString ( char * Output )
			{
				
				uint32_t RegOut [ 4 ];
				
				GetCPUID ( 0, RegOut );
				
				* reinterpret_cast <uint32_t *> ( & Output [ 0 ] ) = RegOut [ kRegister_EBX ];
				* reinterpret_cast <uint32_t *> ( & Output [ 4 ] ) = RegOut [ kRegister_EDX ];
				* reinterpret_cast <uint32_t *> ( & Output [ 8 ] ) = RegOut [ kRegister_ECX ];
				
			};
			
		};
		
	};
	
};

#endif
