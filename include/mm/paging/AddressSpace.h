#ifndef MM_PAGING_ADDRESSSPACE_H
#define MM_PAGING_ADDRESSSPACE_H

#include <boot/multiboot/multiboot.h>

#include <mm/MM.h>
#include <mm/paging/Paging.h>

#include <math/bitmath.h>

#include <stdint.h>
#include <stddef.h>

#include <system/func/kprintf.h>

namespace MM
{
	
	namespace Paging
	{
		
		class AddressSpace
		{
		public:
			
			static const uint32_t kCreateAddressSpace_Error_None = 0;
			static const uint32_t kCreateAddressSpace_Error_InitialZoneSize = 1;
			static const uint32_t kCreateAddressSpace_Error_FailedPhysicalAllocation = 2;
			
			static const uint32_t kAlloc_Error_None = 0;
			static const uint32_t kAlloc_Error_FailedPhysicalAllocation = 2;
			static const uint32_t kAlloc_Error_OutOfKVirtualSpace = 3;
			static const uint32_t kAlloc_Error_OutOfVirtualSpace = 4;
			static const uint32_t kAlloc_Error_NonKernelAllocUnimplemented = 5;
			
			static const uint32_t kError_Free_NotAllocated = 6;
			static const uint32_t kFree_Error_None = 0;
			
			static const uint32_t kMaxErrorNumber = 6;
			
			static void KInit ( uint32_t KernelBase, uint32_t KernelMemSize );
			
			static void CreateAddressSpace ( AddressSpace * Space, bool Kernel, uint32_t InitialFreeBase, uint32_t IntialFreeLength, uint32_t * Error );
			static const char * GetErrorString ( uint32_t Error );
			
			void * operator new ( size_t, void * Address );
			
			~AddressSpace ();
			
			void AddFreeRange ( uint32_t Base, uint32_t Length );
			
			static AddressSpace * ReteriveKernelSpace ();
			
			void Alloc ( uint32_t Length, void ** Base, uint32_t * Error );
			void Free ( void * Base, uint32_t * Error );
			
			void DebugPrint ();
			
		private:
			
			struct Storage_struct;
			
			typedef struct AddressRange_Struct
			{
				
				uint32_t Base;
				uint32_t Length;
				
				int32_t Height;
				
				AddressRange_Struct * Parent;
				AddressRange_Struct * Left;
				AddressRange_Struct * Right;
				
				AddressRange_Struct * NextInSizeClass;
				AddressRange_Struct * PrevInSizeClass;
				
				uint32_t SSIndex;
				
			} __attribute__((packed)) AddressRange; // 32 bytes
			
			typedef struct Storage_Struct
			{
				
				Storage_Struct * Previous;
				Storage_Struct * Next;
				
				uint32_t Bitmap [ 4 ];
				
				AddressRange Ranges [ 112 ];
				
				uint32_t FreeCount;
				
			} __attribute__((packed)) Storage;
			
			AddressSpace ();
			
			void InsertFreeNode ( AddressRange * Node );
			void InsertAllocatedNode ( AddressRange * Node );
			
			void RemoveFreeNode ( AddressRange * Node );
			void RemoveAllocatedNode ( AddressRange * Node );
			
			AddressRange * Balance ( AddressRange * Root );
			
			AddressRange * FindBest ( uint32_t SizeClass );
			
			AddressRange * FindFreeNode ( uint32_t Address );
			AddressRange * FindAllocatedNode ( uint32_t Address );
			
			AddressRange * FindClosestSmallerFreeNode ( AddressRange * Node );
			
			AddressRange * DoAlloc ( uint32_t RequestedSize );
			
			AddressRange * GetNewRange ();
			void FreeOldRange ( AddressRange * Range );
			
			static const char * ErrorStrings [];
			
			static const uint32_t kAddressRangePTR_Invalid = 0xFFFFFFFF;
			static const uint32_t kStoragePTR_Invalid = 0xFFFFFFFF;
			
			
			AddressRange * RootFreeNode;
			AddressRange * RootAllocatedNode;
			
			AddressRange * FreeSizeClassHeads [ 19 ];
			
			uint32_t FreeNodeCount;
			uint32_t AllocatedNodeCount;
			
			Storage * FreeStorageHead;
			Storage * FullStorageHead;
			
			uint32_t FreeStorageSlotCount;
			uint32_t AllocatedStorageCount;
			
			uint32_t FreeStorageHighWatermark;
			
			uint32_t FreePageCount;
			uint32_t TotalPageCount;
			
			bool Kernel;
			
			static char KernelAddressSpaceMem [];
			static AddressSpace * KernelAddressSpace;
			
			static inline void __TreePrint ( AddressRange * Node )
			{
				
				if ( Node == reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
					return;
				
				__TreePrint ( Node -> Left );
				system_func_kprintf ( "%h, ", Node -> Base );
				__TreePrint ( Node -> Right );
				
			};
			
			static inline int32_t __max ( int32_t A, int32_t B )
			{
				
				return ( A > B ) ? A : B;
				
			};
			
			static inline void __CalcHeight ( AddressRange * Node )
			{
				
				Node -> Height = __max ( ( Node -> Left != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) ) ? Node -> Left -> Height : - 1, ( Node -> Right != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) ) ? Node -> Right -> Height : - 1 ) + 1;
				
			};
			
			static inline AddressRange * __RotateLeft ( AddressRange * Root )
			{
				
				AddressRange * NewRoot = Root -> Right;
				
				if ( Root -> Parent != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
				{
					
					if ( Root -> Parent -> Right == Root ) 
						Root -> Parent -> Right = NewRoot; 
					else
						Root -> Parent -> Left = NewRoot;
					
				}
				
				NewRoot -> Parent = Root -> Parent;
				Root -> Parent = NewRoot;
				Root -> Right = NewRoot -> Left;
				
				if ( Root -> Right != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
					Root -> Right -> Parent = Root;
				
				NewRoot -> Left = Root;
				
				__CalcHeight ( Root );
				__CalcHeight ( NewRoot );
				
				return NewRoot;
				
			};
			
			static inline AddressRange * __RotateRight ( AddressRange * Root )
			{
				
				AddressRange * NewRoot = Root -> Left;
				
				if ( Root -> Parent != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
				{
					
					if ( Root -> Parent -> Left == Root ) 
						Root -> Parent -> Left = NewRoot; 
					else
						Root -> Parent -> Right = NewRoot;
					
				}
				
				NewRoot -> Parent = Root -> Parent;
				Root -> Parent = NewRoot;
				Root -> Left = NewRoot -> Right;
				
				if ( Root -> Left != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
					Root -> Left -> Parent = Root;
				
				NewRoot -> Right = Root;
				
				__CalcHeight ( Root );
				__CalcHeight ( NewRoot );
				
				return NewRoot;
				
			};
			
			static inline AddressRange * __LeftMostNode ( AddressRange * Root )
			{
				
				while ( Root -> Left != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
					Root = Root -> Left;
				
				return Root;
				
			};
			
			static inline uint32_t __CalculateSizeClass ( uint32_t Length )
			{
				
				return 19 - math_bitmath_clz ( Length );
				
			};
			
			static inline uint32_t __StorageAndSlotToIndex ( Storage * Storage, uint32_t Slot )
			{
				
				return ( reinterpret_cast <uint32_t> ( Storage ) & 0xFFFFF000 ) | ( Slot & 0xFFF );
				
			};
			
			static inline Storage * __StorageFromIndex ( uint32_t Index )
			{
				
				return reinterpret_cast <Storage *> ( Index & 0xFFFFF000 );
				
			};
			
			static inline uint32_t __SlotFromIndex ( uint32_t Index )
			{
				
				return ( Index & 0xFFF );
				
			};
			
			static inline void __MarkStorageSlotUsed ( Storage * StorageUnit, uint32_t Slot )
			{
				
				Slot ++;
				
				StorageUnit -> Bitmap [ Slot >> 5 ] |= ( 1 << ( Slot & 0x1F ) );
				
			};
			
			static inline void __MarkStorageSlotClear ( Storage * StorageUnit, uint32_t Slot )
			{
				
				Slot ++;
				
				StorageUnit -> Bitmap [ Slot >> 5 ] &= ~ ( 1 << ( Slot & 0x1F ) );
				
			};
			
			static inline uint32_t __GetFirstFreeStorageSlot ( Storage * StorageUnit )
			{
				
				uint32_t I;
				
				if ( StorageUnit -> Bitmap [ 0 ] != 0xFFFFFFFF )
				{
					
					for ( I = 1; I < 32; I ++ )
					{
						
						if ( ( StorageUnit -> Bitmap [ 0 ] & ( 1 << I ) ) == 0 )
							return I - 1;
						
					}
					
				}
				
				if ( StorageUnit -> Bitmap [ 1 ] != 0xFFFFFFFF )
				{
					
					for ( I = 0; I < 32; I ++ )
					{
						
						if ( ( StorageUnit -> Bitmap [ 1 ] & ( 1 << I ) ) == 0 )
							return I + 31;
						
					}
					
				}
				
				if ( StorageUnit -> Bitmap [ 2 ] != 0xFFFFFFFF )
				{
					
					for ( I = 0; I < 32; I ++ )
					{
						
						if ( ( StorageUnit -> Bitmap [ 1 ] & ( 1 << I ) ) == 0 )
							return I + 63;
						
					}
					
				}
				
				if ( StorageUnit -> Bitmap [ 3 ] != 0xFFFFFFFF )
				{
					
					for ( I = 0; I < 32; I ++ )
					{
						
						if ( ( StorageUnit -> Bitmap [ 1 ] & ( 1 << I ) ) == 0 )
							return I + 95;
						
					}
					
				}
				
				return 0xFFFFFFFF;
				
			};
			
		};
		
	};
	
};

#endif
