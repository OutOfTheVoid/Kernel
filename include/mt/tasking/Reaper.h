#ifndef MT_TASKING_REAPER_H
#define MT_TASKING_REAPER_H

#include <cpputil/Linkage.h>

#include <mt/tasking/Tasking.h>
#include <mt/tasking/Task.h>
#include <mt/synchronization/Spinlock.h>

ASM_LINKAGE void mt_tasking_reaperInit ();

namespace MT
{
	
	namespace Tasking
	{
		
		class Reaper
		{
		public:
			
			static void Init ();
			
			static void Signal ();
			
		private:
			
			friend void ::mt_tasking_reaperInit ();
			
			static void Run ();
			
			static Task * ReaperTask;
			
			static Synchronization::Spinlock :: Spinlock_t TaskLock;
			
		};
		
	};
	
};

#endif
