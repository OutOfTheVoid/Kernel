#ifndef MT_PROCESS_USERPROCESS_H
#define MT_PROCESS_USERPROCESS_H

#include <mt/process/Process.h>

#include <mt/synchronization/Spinlock.h>

#include <system/userspace/Session.h>

#include <util/Vector.h>

namespace MM
{

	namespace Paging
	{

		class AddressSpace;
		class PageTable;

	};

};

namespace MT
{
	
	namespace Tasking
	{
		
		class Task;
		
	}
	
	namespace Process
	{
		
		class UserProcess
		{
		public:
			
			char Name [ 256 ];
			
			uint64_t ProcessID;
			
			MM::Paging::PageTable * MemoryMapping;
			MM::Paging::AddressSpace * MemorySpace;
			
			UserProcess * Parent;
			
			Vector <volatile Tasking :: Task *> * Threads;
			
			MT::Synchronization::Spinlock :: Spinlock_t ThreadListLock;
			
			static void Init ();
			
			static UserProcess * CreateUserProcess ( const char * Name, MM::Paging::PageTable * MemoryMapping, MM::Paging::AddressSpace * MemorySpace, UserProcess * Parent );
			
		private:
			
			static uint64_t GetNewProcessID ();
			
			static Synchronization::Spinlock :: Spinlock_t ProcessIDLock;
			
			static uint64_t MaxProcessID;
			
		};
		
	};
	
};

#endif
