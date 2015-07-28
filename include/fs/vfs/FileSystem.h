#ifndef FS_VFS_FILESYSTEM_H
#define FS_VFS_FILESYSTEM_H

#include <fs/vfs/VFS.h>

#include <stdint.h>

#include <util/Vector.h>
#include <mt/synchronization/Mutex.h>

namespace FS
{
	
	namespace VFS
	{
		
		class FileSystem
		{
		public:
			
			typedef uint32_t FSStatus_t;
			
			static const FSStatus_t kFSStatus_NoFunction = 0;
			
			static const FSStatus_t kFSStatus_Success = 1;
			
			static const FSStatus_t kFSStatus_Failure_Arbitrary = 2;
			
			static const FSStatus_t kFSStatus_Failure_Existance = 3;
			
			static const FSStatus_t kFSStatus_Failure_Position = 4;
			static const FSStatus_t kFSStatus_Failure_Buffer = 5;
			static const FSStatus_t kFSStatus_Failure_Length = 6;
			
			static const FSStatus_t kFSStatus_Failure_SpecialDirectory = 7;
			
			static const FSStatus_t kFSStatus_Failure_ReadDisallowed = 8;
			static const FSStatus_t kFSStatus_Failure_WriteDisallowed = 9;
			
			static const FSStatus_t kFSStatus_Failure_Busy = 10;
			
			static const FSStatus_t kFSStatus_Failure_WrongFS = 11;
			
			static const FSStatus_t kFSStatus_Failure_ReplacesExisting = 10;
			
			
			static const uint32_t kNodeTypes = 2;
			
			static const uint32_t kFSNodeType_File = 0;
			static const uint32_t kFSNodeType_Directory = 1;
			
			static const uint32_t kFSNodeTypeFlag_IllegalNodeType = 0x0;
			static const uint32_t kFSNodeTypeFlag_OpenClose = 0x01;
			static const uint32_t kFSNodeTypeFlag_Read = 0x02;
			static const uint32_t kFSNodeTypeFlag_Write = 0x04;
			static const uint32_t kFSNodeTypeFlag_Enumerate = 0x08;
			static const uint32_t kFSNodeTypeFlag_Find = 0x10;
			static const uint32_t kFSNodeTypeFlag_Add = 0x20;
			static const uint32_t kFSNodeTypeFlag_Delete = 0x40;
			
			struct FSFunctionBlock_Struct;
			
			typedef struct FSNode_Struct
			{
				
				const char * Name;
				
				uint32_t FSSignature;
				uint32_t FSNodeType;
				struct FSFunctionBlock_Struct * FSFunctions;
				
				MT::Synchronization::Mutex :: Mutex_t Lock;
				
			} FSNode;
			
			typedef struct FSFunctionBlock_Struct
			{
				
				uint32_t NodeTypeFlags [ kNodeTypes ];
				
				void ( * Open ) ( FSNode * Node, FSStatus_t * Status );
				void ( * Close ) ( FSNode * Node, FSStatus_t * Status );
				
				void ( * Read ) ( FSNode * Node, uint8_t * Buffer, uint32_t Position, uint32_t Length, FSStatus_t * Status );
				void ( * Write ) ( FSNode * Node, uint8_t * Buffer, uint32_t Position, uint32_t Length, FSStatus_t * Status );
				
				void ( * Enumerate ) ( FSNode * Node, uint32_t ChildIndex, const char ** ChildNamePTR, FSStatus_t * Status );
				void ( * Find ) ( FSNode * Node, const char * Name, FSNode ** ChildPTR, FSStatus_t * Status );
				void ( * Add ) ( FSNode * Node, FSNode * ToAdd, FSStatus_t * Status );
				void ( * Delete ) ( FSNode * Node, FSNode * ToDelete, FSStatus_t * Status );
				
			} FSFunctionBlock;
			
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
