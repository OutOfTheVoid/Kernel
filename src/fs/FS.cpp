#include <fs/FS.h>

#include <fs/vfs/FileSystem.h>
#include <fs/mountfs/MountFS.h>
#include <fs/qmfs/QMFS.h>

#include <fs/initrd/InitRamDisk.h>

#include <system/func/kprintf.h>

#include <util/string/string.h>

void FS :: Init ( multiboot_info_t * MultibootInfo )
{
	
	VFS :: Init ();
	MountFS :: Init ();
	
	QMFS :: Init ();
	
	FS::InitRD::InitRamDisk :: Init ( MultibootInfo );
	
};

FS :: FSNode * ResolvePath ( const char * Path )
{
	
	void ( * FindPTR ) ( FS :: FSNode * Node, const char * Name, FS :: FSNode ** ChildPTR, FS :: FSStatus_t * Status );
	FS :: FSStatus_t FindStatus;
	
	FS :: FSNode * Node = FS::VFS::FileSystem :: GetRootNode ();
	
	if ( * Path == '/' )
		Path ++;
	
	if ( * Path == '\0' )
		return Node;
	
	MT::Synchronization::RWLock :: ReadAcquire ( & Node -> Lock );
	
	while ( true )
	{
		
		FindPTR = Node -> FSInstance -> FSFunctions -> Find;
		
		uint32_t I = 0;
		uint32_t SubNameLength;
		
		const char * TempPath = Path;
		
		while ( ( TempPath [ I ] != '/' ) && ( TempPath [ I ] != '\0' ) )
			I ++;
		
		Path = & TempPath [ I ];
		
		if ( * Path == '/' )
			Path ++;
		
		SubNameLength = I;
		
		char SubName [ SubNameLength + 1 ];
		memcpy ( SubName, TempPath, SubNameLength );
		SubName [ SubNameLength ] = '\0';
		
		FS :: FSNode * Next;
		
		FindPTR ( Node, const_cast <const char *> ( SubName ), & Next, & FindStatus );
		
		if ( FindStatus != FS :: kFSStatus_Success )
		{
			
			MT::Synchronization::RWLock :: ReadRelease ( & Node -> Lock );
			
			return NULL;
			
		}
		
		if ( * Path == '\0' )
		{
			
			MT::Synchronization::RWLock :: ReadRelease ( & Node -> Lock );
			
			return Next;
			
		}
		
		MT::Synchronization::RWLock :: ReadAcquire ( & Next -> Lock );
		MT::Synchronization::RWLock :: ReadRelease ( & Node -> Lock );
		
		Node = Next;
		
	}
	
};

void FS :: Open ( const char * Path, KFile * FileInstancePTR, FSStatus_t * Status )
{
	
	FS :: FSNode * FileNode = ResolvePath ( Path );
	
	if ( FileNode == NULL )
	{
		
		* FileInstancePTR = NULL;
		* Status = kFSStatus_Failure_Nonexistent;
		
		return;
		
	}
	
	if ( ( FileNode -> FSInstance -> FSFunctions -> NodeTypeFlags [ FileNode -> FSNodeType ] & kFSNodeTypeFlag_OpenClose ) == 0 )
	{
		
		* FileInstancePTR = NULL;
		* Status = kFSStatus_Failure_NonFile;
		
		return;
		
	}
	
	FileNode -> FSInstance -> FSFunctions -> Open ( FileNode, Status );
	
	* FileInstancePTR = FileNode;
	
};

void FS :: Close ( KFile FileInstancePTR, FSStatus_t * Status )
{
	
	if ( ( FileInstancePTR -> FSInstance -> FSFunctions -> NodeTypeFlags [ FileInstancePTR -> FSNodeType ] & kFSNodeTypeFlag_OpenClose ) == 0 )
	{
		
		* Status = kFSStatus_Failure_NonFile;
		
		return;
		
	}
	
	FileInstancePTR -> FSInstance -> FSFunctions -> Close ( FileInstancePTR, Status );
	
};

void FS :: Read ( KFile FileInstancePTR, uint8_t * Buffer, uint32_t Position, uint32_t Length, FSStatus_t * Status )
{
	
	( void ) FileInstancePTR;
	( void ) Buffer;
	( void ) Position;
	( void ) Length;
	( void ) Status;
	
};

void FS :: Enumerate ( const char * Path, uint32_t Index, const char ** Name, FSStatus_t * Status )
{
	
	FS :: FSNode * FileNode = ResolvePath ( Path );
	
	if ( FileNode == NULL )
	{
		
		* Status = kFSStatus_Failure_Nonexistent;
		
		return;
		
	}
	
	if ( ( FileNode -> FSInstance -> FSFunctions -> NodeTypeFlags [ FileNode -> FSNodeType ] & kFSNodeTypeFlag_Enumerate ) == 0 )
	{
		
		* Status = kFSStatus_Failure_NonDirectory;
		
		return;
		
	}
	
	FileNode -> FSInstance -> FSFunctions -> Enumerate ( FileNode, Index, Name, Status );
	
};

