#ifndef FS_FS_H
#define FS_FS_H

#include <hw/storage/StorageDevice.h>

#include <mt/synchronization/RWLock.h>

namespace FS
{
	
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
	
	static const FSStatus_t kFSStatus_Failure_ReplacesExisting = 12;
	
	static const FSStatus_t kFSStatus_Failure_DeviceFormat = 13;
	
	static const FSStatus_t kFSStatus_Failure_NonDirectory = 14;
	static const FSStatus_t kFSStatus_Failure_NonFile = 15;
	
	static const FSStatus_t kFSStatus_Failure_StorageDevice = 16;
	
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
		uint32_t FSNodeType;
		
		MT::Synchronization::RWLock :: RWLock_t Lock;
		
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
	
	struct FileSystem_Instance_Struct;
	
	typedef struct FileSystem_Instance_Struct FileSystem_Instance;
	
	typedef struct
	{
		
		const char * Name;
		
		bool ( * TestStorageDevice ) ( HW::Storage::StorageDevice * Device );
		FileSystem_Instance * ( * MountStorageDevice ) ( HW::Storage::StorageDevice * Device );
		
	} FileSystemEntry;
	
	struct FileSystem_Instance_Struct
	{
		
		FileSystemEntry * FSEntry;
		HW::Storage::StorageDevice * Device;
		
		FSNode * RootNode;
		FSFunctionBlock * FSFunctions;
		
	};
	
	void Init ();
	
};

#endif
