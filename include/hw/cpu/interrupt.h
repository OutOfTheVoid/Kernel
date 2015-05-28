#ifndef HW_CPU_INTERRUPT_H
#define HW_CPU_INTERRUPT_H

inline void hw_cpu_cli ()
{
	
	asm ( "cli" );
	
};

inline void hw_cpu_sei ()
{
	
	asm ( "sti" );
	
};

#endif
