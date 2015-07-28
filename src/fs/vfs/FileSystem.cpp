#include <fs/vfs/FileSystem.h>
#include <fs/FS.h>

#include <cpputil/Memory.h>
#include <cpputil/Unused.h>

#include <util/string/String.h>

#include <system/func/KPrintF.h>

FS::VFS::FileSystem :: VFS_VirtualDirectory_FSNode_Struct * FS::VFS::FileSystem :: RootDirectory = NULL;

FS :: FSFunctionBlock FS::VFS::FileSystem :: Functions;

void FS::VFS::FileSystem :: Init ()
{
	
	RootDirectory = new VFS_VirtualDirectory_FSNode_Struct ();
	RootDirectory -> Lock = MT::Synchronization::Mutex :: Initializer ();
	RootDirectory -> FSSignature = VFSSignature;
	RootDirectory -> FSNodeType = kFSNodeType_Directory;
	RootDirectory -> FSFunctions = & Functions;
	
	memzero ( reinterpret_cast <void *> ( & Functions ), sizeof ( Functions ) );
	
	for ( uint32_t I = 0; I < kNodeTypes; I ++ )
		Functions.NodeTypeFlags [ I ] = kFSNodeTypeFlag_IllegalNodeType;
	
	Functions.NodeTypeFlags [ kFSNodeType_Directory ] = kFSNodeTypeFlag_OpenClose | kFSNodeTypeFlag_Enumerate | kFSNodeTypeFlag_Find | kFSNodeTypeFlag_Add | kFSNodeTypeFlag_Delete;
	
	Functions.Open = & Open;
	Functions.Close = & Close;
	Functions.Enumerate = Enumerate;
	Functions.Find = Find;
	Functions.Add = & Add;
	Functions.Delete = Delete;
	
	system_func_kprintf ( "vfs initialized!\n" );
	
};

FS :: FSNode * FS::VFS::FileSystem :: GetRootNode ()
{
	
	return reinterpret_cast <FSNode *> ( RootDirectory );
	
};

void FS::VFS::FileSystem :: Open ( FSNode * Node, FSStatus_t * Status )
{
	
	UNUSED ( Node );
	
	* Status = kFSStatus_Success;
	
};

void FS::VFS::FileSystem :: Close ( FSNode * Node, FSStatus_t * Status )
{
	
	UNUSED ( Node );
	
	* Status = kFSStatus_Success;
	
};
			
void FS::VFS::FileSystem :: Enumerate ( FSNode * Node, uint32_t ChildIndex, const char ** ChildNamePTR, FSStatus_t * Status )
{
	
	if ( Node -> FSSignature != VFSSignature )
	{
		
		* Status = kFSStatus_Failure_WrongFS;
		
		return;
		
	}
	
	MT::Synchronization::Mutex :: Acquire ( & Node -> Lock );
	
	if ( reinterpret_cast <VFS_VirtualDirectory_FSNode *> ( Node ) -> Children.Length () > ChildIndex )
	{
		
		* ChildNamePTR = reinterpret_cast <VFS_VirtualDirectory_FSNode *> ( Node ) -> Children [ ChildIndex ] -> Name;
		* Status = kFSStatus_Success;
		
	}
	else
	{
		
		* ChildNamePTR = NULL;
		* Status = kFSStatus_Failure_Existance;
		
	}
	
	MT::Synchronization::Mutex :: Release ( & Node -> Lock );
	
};

void FS::VFS::FileSystem :: Find ( FSNode * Node, const char * Name, FSNode ** ChildPTR, FSStatus_t * Status )
{
	
	if ( Node -> FSSignature != VFSSignature )
	{
		
		* Status = kFSStatus_Failure_WrongFS;
		
		return;
		
	}
	
	MT::Synchronization::Mutex :: Acquire ( & Node -> Lock );
	
	uint32_t I;
	
	for ( I = 0; I < reinterpret_cast <VFS_VirtualDirectory_FSNode *> ( Node ) -> Children.Length (); I ++ )
	{
		
		if ( strcmp ( reinterpret_cast <VFS_VirtualDirectory_FSNode *> ( Node ) -> Children [ I ] -> Name, Name ) == 0 )
		{
			
			* ChildPTR = reinterpret_cast <VFS_VirtualDirectory_FSNode *> ( Node ) -> Children [ I ];
			
			* Status = kFSStatus_Success;
			
			break;
			
		}
		
	}
	
	if ( I == reinterpret_cast <VFS_VirtualDirectory_FSNode *> ( Node ) -> Children.Length () )
		* Status = kFSStatus_Failure_Existance;
	
	MT::Synchronization::Mutex :: Release ( & Node -> Lock );
	
};

void FS::VFS::FileSystem :: Add ( FSNode * Node, FSNode * ToAdd, FSStatus_t * Status )
{
	
	if ( Node -> FSSignature != VFSSignature )
	{
		
		* Status = kFSStatus_Failure_WrongFS;
		
		return;
		
	}
	
	MT::Synchronization::Mutex :: Acquire ( & Node -> Lock );
	
	uint32_t I;
	
	for ( I = 0; I < reinterpret_cast <VFS_VirtualDirectory_FSNode *> ( Node ) -> Children.Length (); I ++ )
	{
		
		if ( strcmp ( reinterpret_cast <VFS_VirtualDirectory_FSNode *> ( Node ) -> Children [ I ] -> Name, ToAdd -> Name ) == 0 )
		{
			
			* Status = kFSStatus_Failure_ReplacesExisting;
			
			break;
			
		}
		
	}
	
	if ( I == reinterpret_cast <VFS_VirtualDirectory_FSNode *> ( Node ) -> Children.Length () )
	{
		
		reinterpret_cast <VFS_VirtualDirectory_FSNode *> ( Node ) -> Children.Push ( ToAdd );
		
		* Status = kFSStatus_Success;
		
	}
	
	MT::Synchronization::Mutex :: Release ( & Node -> Lock );
	
};

void FS::VFS::FileSystem :: Delete ( FSNode * Node, FSNode * ToDelete, FSStatus_t * Status )
{
	
	if ( Node -> FSSignature != VFSSignature )
	{
		
		* Status = kFSStatus_Failure_WrongFS;
		
		return;
		
	}
	
	MT::Synchronization::Mutex :: Acquire ( & Node -> Lock );
	
	int32_t I = reinterpret_cast <VFS_VirtualDirectory_FSNode *> ( Node ) -> Children.IndexOf ( ToDelete );
	
	if ( I == - 1 )
	{
		
		* Status = kFSStatus_Failure_Existance;
		
	}
	else
	{
		
		reinterpret_cast <VFS_VirtualDirectory_FSNode *> ( Node ) -> Children.Remove ( I, 1 );
		
		* Status = kFSStatus_Success;
		
	}
	
	MT::Synchronization::Mutex :: Release ( & Node -> Lock );
	
};
