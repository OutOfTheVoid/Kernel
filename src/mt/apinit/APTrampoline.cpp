#include <mt/apinit/APTrampoline.h>
#include <mt/apinit/APMain.h>

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
	
};

void MT::APInit::APTrampoline :: SetStack ( void * Stack )
{
	
	Binary -> InitStackTop = reinterpret_cast <uint32_t> ( Stack );
	
};

void * MT::APInit::APTrampoline :: GetEntry ()
{
	
	return reinterpret_cast <void *> ( Binary -> Trampoline );
	
};

