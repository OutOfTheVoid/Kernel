#include <mm/Segmentation/GDT.h>

#include <cpputil/linkage.h>

#include <mm/KMalloc.h>
#include <mm/Paging/PageTable.h>

#include <system/func/Panic.h>
#include <system/func/KPrintf.h>

C_LINKAGE void mm_segmentation_gdtLoad ();

C_LINKAGE MM::Segmentation::GDT :: GDTR Kernel_GDTR;
C_LINKAGE MM::Segmentation::GDT :: GDTR Kernel_OLD_GDTR;

MM::Segmentation::GDT :: GDTR Kernel_GDTR = MM::Segmentation::GDT :: GDTR ();
MM::Segmentation::GDT :: GDTR Kernel_OLD_GDTR = MM::Segmentation::GDT :: GDTR ();

bool MM::Segmentation::GDT :: OwnsCurrent = false;

uint32_t MM::Segmentation::GDT :: EntryCount;
MM::Segmentation::GDT :: GDTEntry * MM::Segmentation::GDT :: Entries;

void MM::Segmentation::GDT :: Init ( uint8_t EntryCount )
{
	
	GDTEntry * Entries = NULL;
	
	if ( EntryCount < 3 )
		EntryCount = 3;
	
	Entries = reinterpret_cast <GDTEntry *> ( mm_kmalloc ( sizeof ( GDTEntry ) * EntryCount ) );
	
	MM::Segmentation::GDT :: EntryCount = EntryCount;
	MM::Segmentation::GDT :: Entries = Entries;
	
	DefineEntry ( & Entries [ 0 ], 0, 0, kAccessType_None, kFlags_None );
	
	SetCodeEntry32 ( 1, 0, 0xFFFFFFFF, 0, true );
	SetDataEntry32 ( 2, 0, 0xFFFFFFFF, 0, true );
	
};

void MM::Segmentation::GDT :: SetDataEntry32 ( uint32_t Index, uint32_t Base, uint32_t Limit, uint8_t Ring, bool Writeable )
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

void MM::Segmentation::GDT :: SetCodeEntry32 ( uint32_t Index, uint32_t Base, uint32_t Limit, uint8_t Ring, bool Readable )
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

void MM::Segmentation::GDT :: Swap ()
{
	
	Kernel_GDTR.Limit = sizeof ( GDTEntry ) * EntryCount - 1;
	Kernel_GDTR.Base = reinterpret_cast <uint32_t> ( Entries );
	
	mm_segmentation_gdtLoad ();
	
	GDTEntry * OldEntries = reinterpret_cast <GDTEntry *> ( Kernel_OLD_GDTR.Base );
	
	if ( OwnsCurrent )
		mm_kfree ( OldEntries );
	
	OwnsCurrent = true;
	
};

void MM::Segmentation::GDT :: DefineEntry ( GDTEntry * Entry, uint32_t Limit, uint32_t Base, AccessType Access, Flags Flag )
{
	
	Entry -> LimitLow = Limit;
	
	Entry -> BaseLow = Base;
	Entry -> BaseMiddle = ( Base & 0xFF0000 ) >> 16;
	
	Entry -> Access = static_cast <uint8_t> ( Access );
	
	Entry -> LimitHigh = ( Limit & 0xF0000 ) >> 16;
	
	Entry -> Flags = static_cast <uint8_t> ( Flag );
	
	Entry -> BaseHigh = ( Base & 0xFF000000 ) >> 24;
	
};
