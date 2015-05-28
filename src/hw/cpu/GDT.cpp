#include <hw/cpu/GDT.h>

#include <cpputil/linkage.h>

#include <mm/paging/PFA.h>
#include <mm/paging/PageTable.h>

#include <system/func/panic.h>

C_LINKAGE void hw_cpu_gdtLoad ();

C_LINKAGE HW::CPU::GDT :: GDTR Kernel_GDTR;
C_LINKAGE HW::CPU::GDT :: GDTR Kernel_OLD_GDTR;

HW::CPU::GDT :: GDTR Kernel_GDTR = HW::CPU::GDT :: GDTR ();
HW::CPU::GDT :: GDTR Kernel_OLD_GDTR = HW::CPU::GDT :: GDTR ();

bool HW::CPU::GDT :: OwnsCurrent = false;
uint32_t HW::CPU::GDT :: EntryCount;

HW::CPU::GDT :: GDTEntry * HW::CPU::GDT :: Entries;

void HW::CPU::GDT :: Init ( uint8_t EntryCount )
{
	
	GDTEntry * Entries = NULL;
	
	if ( EntryCount < 3 )
		EntryCount = 3;
	
	HW::CPU::GDT :: EntryCount = EntryCount;
	
	if ( ! MM::Paging::PFA :: Alloc ( sizeof ( GDTEntry ) * EntryCount, reinterpret_cast <void **> ( & Entries ) ) )
		KPANIC ( "Failed to allocate page for GDT!" );
	
	MM::Paging::PageTable :: SetKernelRegionMapping ( reinterpret_cast <uint32_t> ( Entries ), reinterpret_cast <uint32_t> ( Entries ), sizeof ( GDTEntry ) * EntryCount, MM::Paging::PageTable :: Flags_Present | MM::Paging::PageTable :: Flags_Writeable | MM::Paging::PageTable :: Flags_Cutsom_KMap );
	
	HW::CPU::GDT :: Entries = Entries;
	
	DefineEntry ( & Entries [ 0 ], 0, 0, kAccessType_None, kFlags_None );
	
	SetCodeEntry32 ( 1, 0, 0xFFFFFFFF, 0, true );
	SetDataEntry32 ( 2, 0, 0xFFFFFFFF, 0, true );
	
};

void HW::CPU::GDT :: SetDataEntry32 ( uint32_t Index, uint32_t Base, uint32_t Limit, uint8_t Ring, bool Writeable )
{
	
	if ( Index > EntryCount )
		KPANIC ( "Attempt to write to out of bounds GDT!" );
	
	AccessType Type = static_cast <AccessType> ( kAccessType_Present | kAccessType_Data | kAccessType_Bit4 | kAccessType_DirectionUp | ( Writeable ? kAccessType_DataWiteable : kAccessType_DataReadOnly ) );
	
	switch ( Ring )
	{
	case 0:
	
		Type = static_cast <AccessType> ( Type | kAccessType_Ring0 );
		
		break;
		
	case 1:
	
		Type = static_cast <AccessType> ( Type | kAccessType_Ring1 );
		
		break;
		
	case 2:
	
		Type = static_cast <AccessType> ( Type | kAccessType_Ring2 );
		
		break;
		
	case 3:
	
		Type = static_cast <AccessType> ( Type | kAccessType_Ring3 );
		
		break;
		
	default:
		KPANIC ( "Invalid privelege ring specified" );
		
	}
	
	Flags Flag = static_cast <Flags> ( ( ( Limit > 0xFFFFF ) ? kFlags_Granularity4KB : kFlags_None ) | kFlags_32Bit );
	
	if ( Limit > 0xFFFFF )
		Limit >>= 12;

	DefineEntry ( & Entries [ Index ], Limit, Base, Type, Flag );
	
};

void HW::CPU::GDT :: SetCodeEntry32 ( uint32_t Index, uint32_t Base, uint32_t Limit, uint8_t Ring, bool Readable )
{
	
	if ( Index > EntryCount )
		KPANIC ( "Attempt to write to out of bounds GDT!" );
	
	AccessType Type = static_cast <AccessType> ( kAccessType_Present | kAccessType_Code | kAccessType_Bit4 | kAccessType_ExMatchingPrivelege | ( Readable ? kAccessType_CodeReadable : kAccessType_CodeExOnly ) );
	
	switch ( Ring )
	{
	case 0:
	
		Type = static_cast <AccessType> ( Type | kAccessType_Ring0 );
		
		break;
		
	case 1:
	
		Type = static_cast <AccessType> ( Type | kAccessType_Ring1 );
		
		break;
		
	case 2:
	
		Type = static_cast <AccessType> ( Type | kAccessType_Ring2 );
		
		break;
		
	case 3:
	
		Type = static_cast <AccessType> ( Type | kAccessType_Ring3 );
		
		break;
		
	default:
		KPANIC ( "Invalid privelege ring specified" );
		
	}
	
	Flags Flag = static_cast <Flags> ( ( ( Limit > 0xFFFFF ) ? kFlags_Granularity4KB : 0 ) | kFlags_32Bit );
	
	if ( Limit > 0xFFFFF )
		Limit /= 0x1000;
	
	DefineEntry ( & Entries [ Index ], Limit, Base, Type, Flag );
	
};

void HW::CPU::GDT :: Swap ()
{
	
	Kernel_GDTR.Limit = sizeof ( GDTEntry ) * EntryCount - 1;
	Kernel_GDTR.Base = reinterpret_cast <uint32_t> ( Entries );
	
	hw_cpu_gdtLoad ();
	
	GDTEntry * OldEntries = reinterpret_cast <GDTEntry *> ( Kernel_OLD_GDTR.Base );
	uint32_t OldLength = Kernel_OLD_GDTR.Limit + 1;
	
	if ( OwnsCurrent )
	{
		
		MM::Paging::PFA :: Free ( OldEntries );
		
		MM::Paging::PageTable :: ClearKernelRegionMapping ( reinterpret_cast <uint32_t> ( Entries ), OldLength );
		
	}
	
	OwnsCurrent = true;
	
};

void HW::CPU::GDT :: DefineEntry ( GDTEntry * Entry, uint32_t Limit, uint32_t Base, AccessType Access, Flags Flag )
{
	
	Entry -> LimitLow = Limit;
	
	Entry -> BaseLow = Base;
	Entry -> BaseMiddle = ( Base & 0xFF0000 ) >> 16;
	
	Entry -> Access = static_cast <uint8_t> ( Access );
	
	Entry -> LimitHigh = ( Limit & 0xF0000 ) >> 16;
	
	Entry -> Flags = static_cast <uint8_t> ( Flag );
	
	Entry -> BaseHigh = ( Base & 0xFF000000 ) >> 24;
	
};
