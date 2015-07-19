#ifndef HW_CPU_EXCEPTIONHANDLERS_H
#define HW_CPU_EXCEPTIONHANDLERS_H

#include <cpputil/linkage.h>
#include <interrupt/InterruptHandlers.h>

ASM_LINKAGE void hw_cpu_exceptionDivideByZero ( Interrupt::InterruptHandlers :: ISRFrame * Frame );
ASM_LINKAGE void hw_cpu_exceptionDebug ( Interrupt::InterruptHandlers :: ISRFrame * Frame );
ASM_LINKAGE void hw_cpu_exceptionNMI ( Interrupt::InterruptHandlers :: ISRFrame * Frame );
ASM_LINKAGE void hw_cpu_exceptionBreakpoint ( Interrupt::InterruptHandlers :: ISRFrame * Frame );
ASM_LINKAGE void hw_cpu_exceptionOverflow ( Interrupt::InterruptHandlers :: ISRFrame * Frame );
ASM_LINKAGE void hw_cpu_exceptionBoundRangeExceded ( Interrupt::InterruptHandlers :: ISRFrame * Frame );
ASM_LINKAGE void hw_cpu_exceptionInvalidOpcode ( Interrupt::InterruptHandlers :: ISRFrame * Frame );
ASM_LINKAGE void hw_cpu_exceptionDeviceUnavailible ( Interrupt::InterruptHandlers :: ISRFrame * Frame );
ASM_LINKAGE void hw_cpu_exceptionDoubleFault ( Interrupt::InterruptHandlers :: ISRFrame * Frame );
ASM_LINKAGE void hw_cpu_exceptionInvalidTSS ( Interrupt::InterruptHandlers :: ISRFrame * Frame );
ASM_LINKAGE void hw_cpu_exceptionNoSegmentPresent ( Interrupt::InterruptHandlers :: ISRFrame * Frame );
ASM_LINKAGE void hw_cpu_exceptionSSFault ( Interrupt::InterruptHandlers :: ISRFrame * Frame );
ASM_LINKAGE void hw_cpu_exceptionGeneralProtection ( Interrupt::InterruptHandlers :: ISRFrame * Frame );
ASM_LINKAGE void hw_cpu_exceptionPageFault ( Interrupt::InterruptHandlers :: ISRFrame * Frame );
ASM_LINKAGE void hw_cpu_exceptionFloatingPoint ( Interrupt::InterruptHandlers :: ISRFrame * Frame );
ASM_LINKAGE void hw_cpu_exceptionAlignmentCheck ( Interrupt::InterruptHandlers :: ISRFrame * Frame );
ASM_LINKAGE void hw_cpu_exceptionMachineCheck ( Interrupt::InterruptHandlers :: ISRFrame * Frame );
ASM_LINKAGE void hw_cpu_exceptionSIMDException ( Interrupt::InterruptHandlers :: ISRFrame * Frame );
ASM_LINKAGE void hw_cpu_exceptionSecurity ( Interrupt::InterruptHandlers :: ISRFrame * Frame );

#endif
