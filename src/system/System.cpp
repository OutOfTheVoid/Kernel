#include <system/System.h>

#include <system/permissions/Permissions.h>
#include <system/userspace/UserSpace.h>

void System :: Init ()
{
	
	Permissions :: Init ();
	UserSpace :: Init ();
	
};
