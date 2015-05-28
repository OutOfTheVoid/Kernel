ENTRY(_start)

SECTIONS
{
	
	. = 1M;

	__kbegin = ABSOLUTE(.);
	
	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.multiboot)
		*(.text)
	}
	
	.rodata BLOCK(4K) : ALIGN (4K)
	{
		*(.rodata)
	}
	
	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}
	
	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.boot_stack)
		*(.page_table)
		*(.bss)
	}
	
	__kend = ABSOLUTE(.);
	
}