#ifndef HW_CPU_INTERRUPT_H
#define HW_CPU_INTERRUPT_H

#define HW_CPU_EFLAGS_INTERRUPT 0x200

inline void hw_cpu_cli ()
{
	
	__asm__ volatile ( "cli" );
	
};

inline void hw_cpu_sti ()
{
	
	__asm__ volatile ( "sti" );
	
};

#endif
