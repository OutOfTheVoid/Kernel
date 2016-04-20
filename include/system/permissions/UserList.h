#ifndef SYSTEM_PERMISSIONS_USERLIST_H
#define SYSTEM_PERMISSIONS_USERLIST_H

#include <system/System.h>
#include <system/permissions/Permissions.h>

#include <system/userspace/Session.h>

#include <mt/synchronization/Spinlock.h>

#include <util/Vector.h>

namespace System
{
	
	namespace Permissions
	{
		
		class UserList
		{
		public:
			
			static const char * kSystemUserName;
			static const char * kNullUserName;
			
			static const uint32_t kUserID_Invalid = 0xFFFFFFFF;
			static const uint32_t kUserID_Null = 0;
			static const uint32_t kUserID_System = 1;
			
			static void Init ();
			
			static uint32_t GetUserID ( const char * Name );
			
			static uint32_t AddUser ( const char * Name, bool System );
			
			static UserSpace::Session :: Session_t * GetSession ( uint32_t UserID );
			
		private:
			
			typedef struct
			{
				
				const char * Name;
				
				MT::Synchronization::Spinlock :: Spinlock_t UserLock;
				
				UserSpace::Session :: Session_t Session;
				
			} User_t;
			
			static Vector <User_t *> * Users;
			
			static MT::Synchronization::Spinlock :: Spinlock_t ListLock;
			
			static User_t SystemUser;
			static User_t NullUser;
			
			
		};
		
	};
	
};

#endif
