#include <mt/apinit/APTrampoline.h>
#include <mt/apinit/APMain.h>

//#include <hw/pc/CMOS.h>
//#include <hw/pc/BIOS.h>

#include <cpputil/Linkage.h>

#include <util/string/String.h>

MT::APInit::APTrampoline :: TrapolineBinary * MT::APInit::APTrampoline :: Binary = reinterpret_cast <MT::APInit::APTrampoline :: TrapolineBinary *> ( 0x1000 );

C_LINKAGE uint32_t __mt_apinit_trampolinestart;
C_LINKAGE uint32_t __mt_apinit_trampolineend;

void MT::APInit::APTrampoline :: Init ()
{
	
	uint32_t BinSize = reinterpret_cast <uint32_t> ( & __mt_apinit_trampolineend ) - reinterpret_cast <uint32_t> ( & __mt_apinit_trampolinestart );
	
	memcpy ( reinterpret_cast <void *> ( Binary ), reinterpret_cast <void *> ( & __mt_apinit_trampolinestart ), BinSize );
	
	Binary -> PModeMainPTR = reinterpret_cast <uint32_t> ( & mt_apinit_apmain );
	
	//HW::PC::CMOS :: Write ( HW::PC::CMOS :: kRegister_System_ShutdownCode, HW::PC::CMOS :: kShutdownCode_JumpNoEOI );
	//HW::PC::BIOS :: ConfigWarmBootJump ( reinterpret_cast <uint32_t> ( Binary ) );
	
};

void MT::APInit::APTrampoline :: SetStack ( void * Stack )
{
	
	Binary -> InitStackTop = reinterpret_cast <uint32_t> ( Stack );
	
};

void MT::APInit::APTrampoline :: SetPagingDirectory ( uint32_t * PagingDirectory )
{
	
	Binary -> PagingDirectory = PagingDirectory;
	
};

