#ifndef FS_VFS_FILESYSTEM_H
#define FS_VFS_FILESYSTEM_H

#include <fs/vfs/VFS.h>
#include <fs/FS.h>

#include <stdint.h>

#include <util/Vector.h>

namespace FS
{
	
	namespace VFS
	{
		
		class FileSystem
		{
		public:
			
			static void Init ();
			
			static FSNode * GetRootNode ();
			
		private:
			
			static const uint32_t VFSSignature = 0x00000001;
			
			typedef struct VFS_VirtualDirectory_FSNode_Struct : public FSNode
			{
				
				Vector <FSNode *> Children;
				
			} VFS_VirtualDirectory_FSNode;
			
			static VFS_VirtualDirectory_FSNode * RootDirectory;
			
			static FSFunctionBlock Functions;
			
			static void Open ( FSNode * Node, FSStatus_t * Status );
			static void Close ( FSNode * Node, FSStatus_t * Status );
			
			static void Enumerate ( FSNode * Node, uint32_t ChildIndex, const char ** ChildNamePTR, FSStatus_t * Status );
			static void Find ( FSNode * Node, const char * Name, FSNode ** ChildPTR, FSStatus_t * Status );
			
			static void Add ( FSNode * Node, FSNode * ToAdd, FSStatus_t * Status );
			static void Delete ( FSNode * Node, FSNode * ToDelete, FSStatus_t * Status );
			
		};
		
	};
	
};

#endif
