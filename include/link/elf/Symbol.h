#ifndef LINK_ELF_SYMBOL_H
#define LINK_ELF_SYMBOL_H

#include <link/elf/Elf.h>

namespace Link
{
	
	namespace Elf
	{
		
		class Symbol
		{
		public:
			
			Symbol ( const char * Name, const char * SectionName, uint32_t Offset );
			~Symbol ();	
			
			
		};
		
	};
	
};

#endif
