#include <interrupt/IOAPIC.h>

#include <hw/acpi/MADT.h>

void Interrupt::IOAPIC :: Init ()
{
	
	
	
};

uint32_t Interrupt::IOAPIC :: ReadRegister ( void * BaseAddress, uint32_t Register )
{
	
	reinterpret_cast <uint32_t *> ( BaseAddress ) [ 0 ] = Register & 0xFF;
	return reinterpret_cast <uint32_t *> ( BaseAddress ) [ 4 ];
	
};

void nterrupt::IOAPIC :: WriteRegister ( void * BaseAddress, uint32_t Register, uint32_t Value )
{
	
	reinterpret_cast <uint32_t *> ( BaseAddress ) [ 0 ] = Register & 0xFF;
	reinterpret_cast <uint32_t *> ( BaseAddress ) [ 4 ] = Value;
	
};
