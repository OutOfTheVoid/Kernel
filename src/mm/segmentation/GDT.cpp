#include <mm/Segmentation/GDT.h>

#include <cpputil/linkage.h>

#include <mm/KMalloc.h>
#include <mm/Paging/PageTable.h>

#include <system/func/Panic.h>
#include <system/func/KPrintf.h>

ASM_LINKAGE void mm_segmentation_gdtLoad ();

C_LINKAGE MM::Segmentation::GDT :: GDTR Kernel_GDTR;
C_LINKAGE MM::Segmentation::GDT :: GDTR Kernel_OLD_GDTR;

MM::Segmentation::GDT :: GDTR Kernel_GDTR = MM::Segmentation::GDT :: GDTR ();
MM::Segmentation::GDT :: GDTR Kernel_OLD_GDTR = MM::Segmentation::GDT :: GDTR ();

bool MM::Segmentation::GDT :: OwnsCurrent = false;

uint32_t MM::Segmentation::GDT :: EntryCount;
MM::Segmentation::GDT :: GDTEntry * MM::Segmentation::GDT :: Entries;

void MM::Segmentation::GDT :: Init ( uint8_t EntryCount )
{
	
	if ( EntryCount < 3 )
		EntryCount = 3;
	
	Entries = reinterpret_cast <GDTEntry *> ( mm_kmalloc ( sizeof ( GDTEntry ) * EntryCount ) );
	
	if ( Entries == NULL )
		KPANIC ( "GDT kmalloc failed!" );
	
	GDT :: EntryCount = EntryCount;
	
	DefineEntry ( & Entries [ 0 ], 0, 0, kAccessType_None, kFlags_None );
	
	SetCodeEntry32 ( 1, 0, 0xFFFFFFFF, 0, true );
	SetDataEntry32 ( 2, 0, 0xFFFFFFFF, 0, true );
	
};

uint8_t MM::Segmentation::GDT :: GetEntryCount ()
{
	
	return EntryCount;
	
};

void MM::Segmentation::GDT :: Expand ( uint8_t EntryCount )
{
	
	if ( ! OwnsCurrent )
		return Init ( EntryCount );
	
	if ( EntryCount == 0 )
		return;
	
	GDTEntry * Entries = reinterpret_cast <GDTEntry *> ( mm_kmalloc ( sizeof ( GDTEntry ) * ( EntryCount ) ) );
	
	if ( Entries == NULL )
		KPANIC ( "GDT kmalloc failed!" );
	
	for ( uint32_t I = 0; I < GDT :: EntryCount; I ++ )
		Entries [ I ] = GDT :: Entries [ I ];
		
	GDT :: Entries = Entries;
	GDT :: EntryCount = EntryCount;
	
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
	
	Flags Flag = static_cast <Flags> ( ( ( Limit > 0xFFFF ) ? kFlags_Granularity4KB : kFlags_None ) | kFlags_32Bit );
	
	if ( Limit > 0xFFFF )
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
	
	Flags Flag = static_cast <Flags> ( ( ( Limit > 0xFFFF ) ? kFlags_Granularity4KB : 0 ) | kFlags_32Bit );
	
	if ( Limit > 0xFFFF )
		Limit >>= 12;
	
	DefineEntry ( & Entries [ Index ], Limit, Base, Type, Flag );
	
};

void MM::Segmentation::GDT :: SetDataEntry16 ( uint32_t Index, uint32_t Base, uint32_t Limit, uint8_t Ring, bool Writeable )
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
	
	Flags Flag = static_cast <Flags> ( ( Limit > 0xFFFFF ) ? kFlags_Granularity4KB : kFlags_None );
	
	if ( Limit > 0xFFFFF )
		Limit >>= 12;

	DefineEntry ( & Entries [ Index ], Limit, Base, Type, Flag );
	
};

void MM::Segmentation::GDT :: SetCodeEntry16 ( uint32_t Index, uint32_t Base, uint32_t Limit, uint8_t Ring, bool Readable )
{
	
	(void) Index;
	(void) Base;
	(void) Limit;
	(void) Ring;
	(void) Readable;
	
};

void MM::Segmentation::GDT :: SetTSSEntry ( uint32_t Index, MT::HW::TSS :: TSS_t * Address, uint8_t Ring )
{
	
	Flags Flag = static_cast <Flags> ( kFlags_None );
	AccessType Type = static_cast <AccessType> ( kAccessType_Accessed | kAccessType_Code | kAccessType_Present );
	
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
	
	DefineEntry ( & Entries [ Index ], sizeof ( MT::HW::TSS :: TSS_t ), reinterpret_cast <uint32_t> ( Address ), Type, Flag );
	
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
