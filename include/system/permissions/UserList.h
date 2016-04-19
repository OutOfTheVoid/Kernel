#ifndef SYSTEM_PERMISSIONS_USERLIST_H
#define SYSTEM_PERMISSIONS_USERLIST_H

#include <system/System.h>
#include <system/permissions/Permissions.h>

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
			
			static void Init ();
			
		private:
			
			typedef struct
			{
				
				const char * Name;
				
			} User_t;
			
			static Vector <User_t *> * Users;
			
			static User_t SystemUser;
			static User_t NullUser;
			
			
		};
		
	};
	
};

#endif
