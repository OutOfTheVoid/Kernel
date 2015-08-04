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
			
			const static uint32_t kCode_Extensions = 1;
			
			const static uint32_t kCode_APMI  = 0x80000007;
			
			const static uint32_t kDFlag_Extensions_FPU = 0x00000001;
			const static uint32_t kDFlag_Extensions_VME = 0x00000002;
			const static uint32_t kDFlag_Extensions_DE = 0x00000004;
			const static uint32_t kDFlag_Extensions_PSE = 0x00000008;
			const static uint32_t kDFlag_Extensions_TSC = 0x00000010;
			const static uint32_t kDFlag_Extensions_MSR = 0x00000020;
			const static uint32_t kDFlag_Extensions_PAE = 0x00000040;
			const static uint32_t kDFlag_Extensions_MCE = 0x00000080;
			const static uint32_t kDFlag_Extensions_CXCHG8 = 0x00000100;
			const static uint32_t kDFlag_Extensions_APIC = 0x00000200;
			const static uint32_t kDFlag_Extensions_SEP = 0x00000800;
			const static uint32_t kDFlag_Extensions_MTRR = 0x00001000;
			const static uint32_t kDFlag_Extensions_PGE = 0x00002000;
			const static uint32_t kDFlag_Extensions_MCA = 0x00004000;
			const static uint32_t kDFlag_Extensions_CMOV = 0x00008000;
			const static uint32_t kDFlag_Extensions_PAT = 0x00010000;
			const static uint32_t kDFlag_Extensions_PSE36 = 0x00020000;
			const static uint32_t kDFlag_Extensions_PSN = 0x00040000;
			const static uint32_t kDFlag_Extensions_CLFL = 0x00080000;
			const static uint32_t kDFlag_Extensions_DTES = 0x00200000;
			const static uint32_t kDFlag_Extensions_ACPI = 0x00400000;
			const static uint32_t kDFlag_Extensions_MMX = 0x00800000;
			const static uint32_t kDFlag_Extensions_FXSR = 0x01000000;
			const static uint32_t kDFlag_Extensions_SSE = 0x02000000;
			const static uint32_t kDFlag_Extensions_SSE2 = 0x04000000;
			const static uint32_t kDFlag_Extensions_SS = 0x08000000;
			const static uint32_t kDFlag_Extensions_HTT = 0x10000000;
			const static uint32_t kDFlag_Extensions_TM1 = 0x20000000;
			const static uint32_t kDFlag_Extensions_IA64 = 0x40000000;
			const static uint32_t kDFlag_Extensions_PBE = 0x80000000;
			
			const static uint32_t kDFlag_APMI_InvariantTSC = 0x0000010;
			
			
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
