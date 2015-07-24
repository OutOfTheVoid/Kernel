#include <hw/pc/ISA.h>

#include <hw/acpi/MADT.h>

#include <system/func/Panic.h>

#include <interrupt/IOAPIC.h>

void HW::PC::ISA :: SetIRQHandler ( uint8_t IRQ, void ( * Handler ) ( Interrupt::InterruptHandlers :: ISRFrame * ) )
{
	
	if ( IRQ > 0x0F )
		KPANIC ( "Attempt to install out of range ISA IRQ Handler!" );
	
	Interrupt::InterruptHandlers :: SetInterruptHandler ( kIRQ_InterruptBase + IRQ, Handler );
	
};

void HW::PC::ISA :: InitIOInterrupts ()
{
	
	uint32_t I;
	uint32_t E;
	
	uint32_t GlobalSystemInterrupt;
	
	bool EdgeTriggered;
	bool ActiveHigh;
	
	for ( I = 0; I < 0x0F; I ++ )
	{
		
		if ( I == 2 )
			continue;
		
		GlobalSystemInterrupt = I;
		
		EdgeTriggered = true;
		ActiveHigh = true;
		
		for ( E = 0; E < HW::ACPI::MADT :: GetInterruptSourceOverrideCount (); E ++ )
		{
			
			if ( ( HW::ACPI::MADT :: GetInterruptSourceOverrideBus ( E ) == kMADT_ISABusNumber ) && ( I == HW::ACPI::MADT :: GetInterruptSourceOverrideSourceIRQ ( E ) ) )
			{
				
				GlobalSystemInterrupt = HW::ACPI::MADT :: GetInterruptSourceOverrideInterrupt ( E );
				
				switch ( HW::ACPI::MADT :: GetInterruptSourceOverrideFlags ( E ) & HW::ACPI::MADT :: kInterruptSourceOverrideRecord_Flags_PolarityMask )
				{
					
				case HW::ACPI::MADT :: kInterruptSourceOverrideRecord_Flags_ActiveLow:
					
					ActiveHigh = false;
					
					break;
					
				case HW::ACPI::MADT :: kInterruptSourceOverrideRecord_Flags_ActiveHigh:
				case HW::ACPI::MADT :: kInterruptSourceOverrideRecord_Flags_ConformsToBusPolarity:
				default:
					
					break;	
					
				}
				
				switch ( HW::ACPI::MADT :: GetInterruptSourceOverrideFlags ( E ) & HW::ACPI::MADT :: kInterruptSourceOverrideRecord_Flags_TriggerMask )
				{
					
				case HW::ACPI::MADT :: kInterruptSourceOverrideRecord_Flags_LevelTriggered:
					
					EdgeTriggered = false;
					
					break;
					
				case HW::ACPI::MADT :: kInterruptSourceOverrideRecord_Flags_EdgeTriggered:
				case HW::ACPI::MADT :: kInterruptSourceOverrideRecord_Flags_ConformsToBusTrigger:
				default:
					
					break;	
					
				}
				
				break;
				
			}
			
		}
		
		Interrupt::IOAPIC :: DefineFixedRedirectionEntry ( GlobalSystemInterrupt, kIRQ_InterruptBase + I, 0, ActiveHigh, EdgeTriggered, true );
		
	}
	
};

void HW::PC::ISA :: SetIRQEnabled ( uint8_t IRQ, bool Enabled )
{
	
	for ( uint32_t E = 0; E < HW::ACPI::MADT :: GetInterruptSourceOverrideCount (); E ++ )
	{
		
		if ( ( HW::ACPI::MADT :: GetInterruptSourceOverrideBus ( E ) == kMADT_ISABusNumber ) && ( IRQ == HW::ACPI::MADT :: GetInterruptSourceOverrideSourceIRQ ( E ) ) )
		{
			
			IRQ = HW::ACPI::MADT :: GetInterruptSourceOverrideInterrupt ( E );
			
			break;
		}
		
	}
	
	Interrupt::IOAPIC :: SetRedirectionEntryEnabled ( IRQ, Enabled );
	
};
