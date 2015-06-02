#include <hw/pci/Config.h>

#include <hw/cpu/IO.h>

uint32_t HW::PCI::Config :: Read32 ( uint32_t Bus, uint32_t Device, uint32_t Function, uint32_t Offset )
{
	
	uint32_t Address = ( ( Bus & 0xFF ) << 16 ) | ( ( Device & 0x1F ) << 11 ) | ( ( Function & 0x07 ) << 8 ) | ( ( Offset & 0x3C ) << 2 );
	
	HW::CPU::IO :: Out32 ( kPort_Address, Address );
	return HW::CPU::IO :: In32 ( kPort_Data );
	
};

uint16_t HW::PCI::Config :: Read16 ( uint32_t Bus, uint32_t Device, uint32_t Function, uint32_t Offset )
{
	
	if ( ( Offset & 1 ) == 1 )
		return Read32 ( Bus, Device, Function, Offset ) >> 16;
	
	return Read32 ( Bus, Device, Function, Offset );
	
};

uint8_t HW::PCI::Config :: Read8 ( uint32_t Bus, uint32_t Device, uint32_t Function, uint32_t Offset )
{
	
	return Read32 ( Bus, Device, Function, Offset & 0xFE ) >> ( 8 * ( Offset & 0x03 ) );
	
};

uint16_t HW::PCI::Config :: CheckVendor ( uint32_t Bus, uint32_t Device )
{
	
	return Read16 ( Bus, Device, 0, 0x00 );
	
};
