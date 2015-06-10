#include <hw/cpu/Processor.h>

#include <interrupt/APIC.h>
#include <interrupt/IState.h>

HW::CPU::Processor :: CPUInfo HW::CPU::Processor :: Infos [ 0x100 ];
uint8_t HW::CPU::Processor :: IndexToAPICID [ 0x100 ];

uint8_t HW::CPU::Processor :: CIndex = 0;

HW::CPU::Processor :: CPUInfo * HW::CPU::Processor :: GetCurrent ()
{
	
	uint32_t APICID = Interrupt::APIC :: GetLocalID ();
	
	return Infos [ APICID ];
	
};

HW::CPU::Processor :: CPUInfo * HW::CPU::Processor :: GetCurrentNP ()
{
	
	uint32_t APICID = Interrupt::APIC :: GetLocalIDNP ();
	
	return Infos [ APICID ];
	
};

void HW::CPU::Processor :: Define ( bool BSP, uint8_t APICID, void * InitStackBottom, uint32_t InitStackLength )
{
	
	Infos [ APICID ].Flags = ( BSP ? ( kCPUFlag_ProtectedMode | kCPUFlag_Paging | kCPUFlag_BSP ) : ( kCPUFlag_RealMode | kCPUFlag_StartingUp ) ) | kCPUFlag_InitKernelStack;
	Infos [ APICID ].CurrentCLICount = BSP ? Interrupt::IState :: GetBlockCount () : 0;
	Infos [ APICID ].APICID = APICID;
	Infos [ APICID ].InitStackBottom = InitStackBottom;
	Infos [ APICID ].InitStackLength = InitStackLength;
	Infos [ APICID ].Index = CIndex;
	
	IndexToAPICID [ CIndex ] = APICID;
	
	CIndex ++;
	
};
