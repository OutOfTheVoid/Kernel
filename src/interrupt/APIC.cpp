#include <interrupt/APIC.h>
#include <interrupt/IState.h>
#include <interrupt/PIC.h>

#include <hw/cpu/CPUID.h>
#include <hw/cpu/MSR.h>

#include <hw/acpi/MADT.h>

#include <system/func/Panic.h>
#include <system/func/KPrintF.h>

#include <mm/KVMap.h>

#include <mm/paging/PageTable.h>

#include <cpputil/linkage.h>

C_LINKAGE uint32_t interrupt_APIC_MMIOLocation;

bool Interrupt::APIC :: Availible = true;
		
uint32_t Interrupt::APIC :: Base = 0;
uint32_t * Interrupt::APIC :: BaseVirtual = NULL;

void Interrupt::APIC :: Init ()
{
	
	if ( ! HasAPIC () )
		system_func_panic ( "System does not report an APIC!" );
	
	// Remap PIC to unused interrupt numbers.
	Interrupt::PIC :: Remap ( 0x50, 0x58 );
	
	for ( uint32_t i = 0; i < 15; i ++ )
		Interrupt::PIC :: SetIRQEnabled ( i, false );
	
	Interrupt::PIC :: Disable ();
	
	system_func_kprintf ( "APIC Base address: %h\n", GetAPICBaseAddress () );
	
	uint32_t BaseAddress = GetAPICBaseAddress ();
	Base = BaseAddress;
		
	BaseVirtual = reinterpret_cast <uint32_t *> ( mm_kvmap ( reinterpret_cast <void *> ( Base ), 0x1000, MM::Paging::PageTable :: Flags_NoCache | MM::Paging::PageTable :: Flags_Writeable ) );
	
	if ( BaseVirtual == NULL )
		KPANIC ( "Failed to allocate virtual memory to map APIC!" );
	
	Disable ();
	
};

void Interrupt::APIC :: APInit ()
{
	
	Disable ();
	
	SetAPICBaseAddress ( GetAPICBaseAddress () | HW::CPU::MSR :: kMSR_APICBASEFlag_Enable );
	
}

void Interrupt::APIC :: ReadRegister ( uint32_t Offset, uint32_t * DataOut, uint32_t DWordLength )
{
	
	if ( BaseVirtual == NULL )
		KPANIC ( "Attempt to read APIC register without first mapping it into memory!" );
	
	for ( uint32_t I = 0; I < DWordLength; I ++ )
		DataOut [ I ] = BaseVirtual [ ( Offset >> 2 ) + I ];
	
};

void Interrupt::APIC :: WriteRegister ( uint32_t Offset, uint32_t * DataIn, uint8_t DWordLength )
{
	
	if ( BaseVirtual == NULL )
		KPANIC ( "Attempt to write APIC register without first mapping it into memory!" );
	
	for ( uint32_t I = 0; I < DWordLength; I ++ )
		BaseVirtual [ ( Offset >> 2 ) + I ] = DataIn [ I ];
	
};

void Interrupt::APIC :: SetAPICBaseAddress ( uint32_t Base )
{
	
	uint32_t EAX = ( Base & 0xFFFFF100 ) | HW::CPU::MSR :: kMSR_APICBASEFlag_Enable;
	uint32_t EDX = 0;
	
	HW::CPU::MSR :: SetMSR ( HW::CPU::MSR :: kMSR_APIC_BASE, & EAX, & EDX );
	
	APIC :: Base = Base;
	
};

uint32_t Interrupt::APIC :: GetAPICBaseAddress ()
{
	
	if ( HW::ACPI::MADT :: Valid () )
		return HW::ACPI::MADT :: GetAPICBaseAddress ();
	
	uint32_t EAX;
	uint32_t EDX;
	
	HW::CPU::MSR :: GetMSR ( HW::CPU::MSR :: kMSR_APIC_BASE, & EAX, & EDX );
	
	return EAX & 0xFFFFF000;
	
};

void Interrupt::APIC :: Enable ()
{
	
	uint32_t SpuriousInterruptRegister;
	
	ReadRegister ( kRegisterOffset_SpuriousInterruptVector, & SpuriousInterruptRegister, 1 );
	
	SpuriousInterruptRegister |= kSpuriousInterruptVectorFlag_APICEnable;
	
	WriteRegister ( kRegisterOffset_SpuriousInterruptVector, & SpuriousInterruptRegister, 1 );
	
};

void Interrupt::APIC :: Disable ()
{
	
	uint32_t SpuriousInterruptRegister;
	
	ReadRegister ( kRegisterOffset_SpuriousInterruptVector, & SpuriousInterruptRegister, 1 );
	
	SpuriousInterruptRegister &= ~ kSpuriousInterruptVectorFlag_APICEnable;
	
	WriteRegister ( kRegisterOffset_SpuriousInterruptVector, & SpuriousInterruptRegister, 1 );
	
};

bool Interrupt::APIC :: IsLocalBSP ()
{
	
	uint32_t EAX;
	uint32_t EDX;
	
	HW::CPU::MSR :: GetMSR ( HW::CPU::MSR :: kMSR_APIC_BASE, & EAX, & EDX );
	
	return ( EAX & 0x100 );
	
};

uint8_t Interrupt::APIC :: GetLocalID ()
{
	
	uint32_t IDRegister;
	
	ReadRegister ( kRegisterOffset_ID, & IDRegister, 1 );
	
	return IDRegister >> 24;
	
};

void Interrupt::APIC :: SetLocalID ( uint8_t ID )
{
	
	uint32_t IDRegister = static_cast <uint32_t> ( ID ) << 24;
	
	WriteRegister ( kRegisterOffset_ID, & IDRegister, 1 );
	
};

uint8_t Interrupt::APIC :: GetLocalVersion ()
{
	
	uint32_t VersionRegister;
	
	ReadRegister ( kRegisterOffset_Version, & VersionRegister, 1 );
	
	return static_cast <uint8_t> ( VersionRegister );
	
};

uint8_t Interrupt::APIC :: GetLocalLVTCount ()
{
	
	uint32_t VersionRegister;
	
	ReadRegister ( kRegisterOffset_Version, & VersionRegister, 1 );
	
	return static_cast <uint8_t> ( VersionRegister >> 16 );
	
};

void Interrupt::APIC :: SetLocalTimerVector ( bool InitiallyMasked, uint8_t Vector )
{
	
	uint32_t LVTEntry = static_cast <uint32_t> ( Vector ) | ( InitiallyMasked ? kLVTFlag_Mask : 0 );
	
	if ( Vector < 16 )
		system_func_panic ( "APIC Timer vector set to illegal interrupt vector %i!", Vector );
	
	WriteRegister ( kRegisterOffset_LVTTimer, & LVTEntry, 1 );
	
};

void Interrupt::APIC :: SetLocalTimerDivide ( uint32_t Divide )
{
	
	uint32_t DivideRegister;
	
	ReadRegister ( kRegisterOffset_TimerDivideConfig, & DivideRegister, 1 );
	DivideRegister &= ~ 0x0000000C;
	
	switch ( Divide )
	{
		
	case kTimerDivision_1:
		DivideRegister |= 0x07;
		break;
		
	case kTimerDivision_2:
		DivideRegister |= 0x00;
		break;
		
	case kTimerDivision_4:
		DivideRegister |= 0x01;
		break;
		
	case kTimerDivision_8:
		DivideRegister |= 0x02;
		break;
		
	case kTimerDivision_16:
		DivideRegister |= 0x03;
		break;
		
	case kTimerDivision_32:
		DivideRegister |= 0x04;
		break;
		
	case kTimerDivision_64:
		DivideRegister |= 0x05;
		break;
		
	case kTimerDivision_128:
		DivideRegister |= 0x06;
		break;
		
	default:
		system_func_panic ( "Attempt to set invalid APIC Timer divison!" );
		
	};
	
	WriteRegister ( kRegisterOffset_TimerDivideConfig, & DivideRegister, 1 );
	
};

void Interrupt::APIC :: StartTimerOneShot ( uint32_t InitialValue )
{
	
	uint32_t LVTEntry;
	bool UnMask;
	
	ReadRegister ( kRegisterOffset_LVTTimer, & LVTEntry, 1 );
	
	if ( ( LVTEntry & kLVTFlag_Mask ) == 0 )
		UnMask = true;
	else
		UnMask = false;
	
	if ( LVTEntry & kLVTFlag_TimerModePeriodic )
	{
		
		LVTEntry &= ~ kLVTFlag_TimerModePeriodic;
		WriteRegister ( kRegisterOffset_LVTTimer, & LVTEntry, 1 );
		
	}
	
	WriteRegister ( kRegisterOffset_TimerInitialCount, & InitialValue, 1 );
	
	if ( UnMask )
	{
		
		LVTEntry &= ~ kLVTFlag_Mask;
		WriteRegister ( kRegisterOffset_LVTTimer, & LVTEntry, 1 );
		
	}
	
};

void Interrupt::APIC :: StartTimerPeriodic ( uint32_t SystemClockPeriod )
{
	
	uint32_t LVTEntry;
	bool UnMask;
	
	ReadRegister ( kRegisterOffset_LVTTimer, & LVTEntry, 1 );
	
	if ( ( LVTEntry & kLVTFlag_Mask ) == 0 )
		UnMask = true;
	else
		UnMask = false;
	
	if ( ( ( LVTEntry & kLVTFlag_TimerModePeriodic ) == 0 ) || UnMask )
	{
		
		LVTEntry |= ( kLVTFlag_TimerModePeriodic | kLVTFlag_Mask );
		WriteRegister ( kRegisterOffset_LVTTimer, & LVTEntry, 1 );
		
	}
	
	WriteRegister ( kRegisterOffset_TimerInitialCount, & SystemClockPeriod, 1 );
	
	if ( UnMask )
	{
		
		LVTEntry &= ~ kLVTFlag_Mask;
		WriteRegister ( kRegisterOffset_LVTTimer, & LVTEntry, 1 );
		
	}
	
};

bool Interrupt::APIC :: PollTimerOneShot ()
{
	
	uint32_t TimerCountRegister;
	
	ReadRegister ( kRegisterOffset_TimerCurrentCount, & TimerCountRegister, 1 );
	
	return ( TimerCountRegister == 0 );
	
};

void Interrupt::APIC :: SendPhysicalInitIPI ( uint8_t TargetID, bool Assert )
{
	
	uint32_t InterruptCommandHighRegister = static_cast <uint32_t> ( TargetID ) << 24;
	uint32_t InterruptCommandLowRegister = kIPIDestinationShorthand_None | kIPITriggerMode_Edge | ( Assert ? kIPILevel_Assert : kIPILevel_DeAssert ) | kIPIDestinationMode_Physical | kIPIDeliveryMode_INIT;
	
	WriteRegister ( kRegisterOffset_InterruptCommand_Upper, & InterruptCommandHighRegister, 1 );
	WriteRegister ( kRegisterOffset_InterruptCommand_Lower, & InterruptCommandLowRegister, 1 );
	
};

void Interrupt::APIC :: SendPhysicalStartUpIPI ( uint8_t TargetID )
{
	
	uint32_t InterruptCommandHighRegister = static_cast <uint32_t> ( TargetID ) << 24;
	uint32_t InterruptCommandLowRegister = kIPIDestinationShorthand_None | kIPITriggerMode_Edge | kIPILevel_Assert | kIPIDestinationMode_Physical | kIPIDeliveryMode_StartUp;
	
	WriteRegister ( kRegisterOffset_InterruptCommand_Upper, & InterruptCommandHighRegister, 1 );
	WriteRegister ( kRegisterOffset_InterruptCommand_Lower, & InterruptCommandLowRegister, 1 );
	
};

bool Interrupt::APIC :: HasAPIC ()
{
	
	if ( Availible == false )
		return false;
	
	if ( HW::ACPI::MADT :: Valid () )
		return true;
	
	uint32_t CPUIDRegs [ 4 ];
	
	HW::CPU::CPUID :: GetCPUID ( 0x01, CPUIDRegs );
	
	Availible = CPUIDRegs [ HW::CPU::CPUID :: kRegister_EDX ] & HW::CPU::CPUID :: kDFlag_APIC;
	return Availible;
	
};


