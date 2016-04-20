#include <system/permissions/UserList.h>

#include <util/string/String.h>

#include <cpputil/Memory.h>

const char * System::Permissions::UserList :: kSystemUserName = "system";
const char * System::Permissions::UserList :: kNullUserName = "_null";

System::Permissions::UserList :: User_t System::Permissions::UserList :: SystemUser;
System::Permissions::UserList :: User_t System::Permissions::UserList :: NullUser;

Vector <System::Permissions::UserList :: User_t *> * System::Permissions::UserList :: Users = NULL;

MT::Synchronization::Spinlock :: Spinlock_t System::Permissions::UserList :: ListLock = MT::Synchronization::Spinlock :: Initializer ();

void System::Permissions::UserList :: Init ()
{
	
	SystemUser.Name = kSystemUserName;
	SystemUser.UserLock = MT::Synchronization::Spinlock :: Initializer ();
	SystemUser.Session = UserSpace::Session :: Initializer ();
	SystemUser.Session.Status = UserSpace::Session :: kSessionStatus_Exists | UserSpace::Session :: kSessionStatus_Active | UserSpace::Session :: kSessionStatus_Scheduling | UserSpace::Session :: kSessionStatus_System;
	
	NullUser.Name = kNullUserName;
	NullUser.UserLock = MT::Synchronization::Spinlock :: Initializer ();
	NullUser.Session = UserSpace::Session :: Initializer ();
	NullUser.Session.Status = UserSpace::Session :: kSessionStatus_Null | UserSpace::Session :: kSessionStatus_System;
	
	Users = new Vector <User_t *> ();
	
	Users -> Push ( & NullUser );
	Users -> Push ( & SystemUser );
	
};

uint32_t System::Permissions::UserList :: GetUserID ( const char * Name )
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & ListLock );
	
	uint32_t ListLength = Users -> Length ();
	uint32_t I;
	
	for ( I = 0; I < ListLength; I ++ )
	{
		
		if ( strcmp ( Name, ( * Users ) [ I ] -> Name ) == 0 )
			break;
		
	}
	
	MT::Synchronization::Spinlock :: Release ( & ListLock );
	
	if ( I >= ListLength )
		return kUserID_Invalid;
	
	return I;
	
};

uint32_t System::Permissions::UserList :: AddUser ( const char * Name, bool System )
{
	
	User_t * New = new User_t ();
	
	New -> Name = Name;
	New -> UserLock = MT::Synchronization::Spinlock :: Initializer ();
	New -> Session = UserSpace::Session :: Initializer ();
	New -> Session.Status = UserSpace::Session :: kSessionStatus_Exists | ( System ? UserSpace::Session :: kSessionStatus_System : 0 );
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & ListLock );
	
	uint32_t ListLength = Users -> Length ();
	
	Users -> Push ( New );
	
	MT::Synchronization::Spinlock :: Release ( & ListLock );
	
	return ListLength;
	
};

System::UserSpace::Session :: Session_t * System::Permissions::UserList :: GetSession ( uint32_t UserID )
{
	
	User_t * User = NULL;
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & ListLock );
	
	if ( UserID < Users -> Length () )
		User = ( * Users ) [ UserID ];
	
	MT::Synchronization::Spinlock :: Release ( & ListLock );
	
	if ( User == NULL )
		return NULL;
	
	return & User -> Session;
	
};

