#include <system/permissions/UserList.h>

const char * System::Permissions::UserList :: kSystemUserName = "system";
const char * System::Permissions::UserList :: kNullUserName = "_null";

System::Permissions::UserList :: User_t System::Permissions::UserList :: SystemUser;
System::Permissions::UserList :: User_t System::Permissions::UserList :: NullUser;

Vector <System::Permissions::UserList :: User_t *> * System::Permissions::UserList :: Users = NULL;

void System::Permissions::UserList :: Init ()
{
	
	SystemUser.Name = kSystemUserName;
	NullUser.Name = kNullUserName;
	
	Users = new Vector <User_t *> ();
	
	Users -> Push ( & SystemUser );
	
};

