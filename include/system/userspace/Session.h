#ifndef SYSTEM_USERSPACE_SESSION_H
#define SYSTEM_USERSPACE_SESSION_H

#include <system/System.h>
#include <system/userspace/UserSpace.h>

#include <stdint.h>

namespace System
{
	
	namespace UserSpace
	{
		
		class Session
		{
		public:
			
			static const uint32_t kSessionStatus_Null = 0;
			static const uint32_t kSessionStatus_Exists = 1;
			static const uint32_t kSessionStatus_Active = 2;
			static const uint32_t kSessionStatus_Scheduling = 4;
			static const uint32_t kSessionStatus_System = 8;
			
			typedef struct
			{
				
				uint32_t Status;
				
			} Session_t;
			
			static inline Session_t Initializer ()
			{
				
				Session_t New;
				
				New.Status = kSessionStatus_Null;
				
				return New;
				
			};
			
		};
		
	};
	
};

#endif
