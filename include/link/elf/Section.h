#ifndef LINK_ELF_SECTION_H
#define LINK_ELF_SECTION_H

#include <link/elf/Elf.h>

namespace Link
{
	
	namespace Elf
	{
		
		class Section
		{
		public:
			
			static const uint32_t FLAG_ZERO = 1;
			
			Section ( const char * Name, uint32_t Flags );
			~Section ();
			
		private:
			
			
			
		};
		
	};
	
};

#endif
