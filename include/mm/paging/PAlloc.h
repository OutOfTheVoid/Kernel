#ifndef MM_PAGING_PALLOC_H
#define MM_PAGING_PALLOC_H

#include <mm/paging/Paging.h>

#include <system/func/kprintf.h>

#include <math/Bitmath.h>
#include <math/IntMath.h>

#include <stdint.h>

namespace MM
{
	
	namespace Paging
	{
		
		class PAlloc
		{
		public:
			
			static const uint32_t kMaxErrorNumber = 0;
			
			typedef void PageAllocZone;
			typedef void PageFreeZone;
			
			static const char * GetErrorString ( uint32_t Error );
			
			static void InitFreeZone ( PageFreeZone ** Zone, const char * Name, uint32_t InitialPhysicalBase, uint32_t InitialPhysicalLength, uint32_t * Error );
			static PageAllocZone * MakePageAllocationZone ( const char * Name, PageFreeZone * FreeZone, uint32_t * Error );
			
			static uint32_t PrePagingCalculateVirtualSize ( PageFreeZone * Zone, uint32_t * Error );
			static void VirtualizeZone ( PageFreeZone * Zone, PageAllocZone * AllocationZones [], uint32_t AllocationZoneCount, uint32_t VirtualBase, uint32_t * Error );
			
		private:
			
			typedef struct AddressRange_Struct
			{
				
				uint32_t Base;
				uint32_t Length;
				
				int32_t Height;
				
				AddressRange_Struct * NextInSizeClass;
				AddressRange_Struct * PreviousInSizeClass;
				
				AddressRange_Struct * Parent;
				AddressRange_Struct * Left;
				AddressRange_Struct * Right;
				
			} __attribute__ (( packed )) AddressRange;
			
			typedef struct Storage_Struct
			{
				
				uint32_t Bitmap [ 4 ];
				
				uint32_t FreeCount;
				
				struct Storage_Struct * Next;
				struct Storage_Struct * Previous;
				
				AddressRange Ranges [ 127 ];
				
			} __attribute__ (( packed )) Storage;
			
			typedef struct
			{
				
				const char * Name;
				
				AddressRange * FreeSizeClassHeads [ 20 ];
				AddressRange * FreeTreeRoot;
				
				Storage * FreeStorageHead;
				Storage * FullStorageHead;
				
			} __attribute__ (( packed )) FreePageZone;
			
			typedef struct
			{
				
				const char * Name;
				
				FreePageZone * FreeZone;
				
				AddressRange * UsedTreeRoot;
				
			} __attribute__ (( packed )) PageAllocationZone;
			
			static const char * ErrorStrings [];
			
			static const uint32_t kAddressRangePTR_Invalid = 0xFFFFFFFF;
			static const uint32_t kStoragePTR_Invalid = 0xFFFFFFFF;
			static const uint32_t kPageFreeZonePTR_Invalid = 0xFFFFFFFF;
			
			static void InsertNode ( AddressRange ** Root, AddressRange * ToInsert );
			static void RemoveNode ( AddressRange ** Root, AddressRange * ToRemove );
			
			static void RemoveNodeFromSizeClass ( FreePageZone * Zone, AddressRange * ToRemove, uint32_t SizeClass );
			static void InsertNodeInSizeClass ( FreePageZone * Zone, AddressRange * ToRemove, uint32_t SizeClass );
			
			static AddressRange * Balance ( AddressRange * LocalRoot );
			
			static AddressRange * FindBest ( FreePageZone * Zone, uint32_t MinimumSizeClass );
			static AddressRange * FindNode ( AddressRange * Root, uint32_t Base );
			
			static AddressRange * FindClosestSmallerNode ( AddressRange * Root, AddressRange * Node );
			
			static AddressRange * NewRange ( FreePageZone * Zone );
			static void FreeRange ( FreePageZone * Zone, AddressRange * Range );
			
			static inline void __TreePrint ( AddressRange * Node )
			{
				
				if ( Node == reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
					return;
				
				__TreePrint ( Node -> Left );
				system_func_kprintf ( "[%h, %h], ", Node -> Base, Node -> Length );
				__TreePrint ( Node -> Right );
				
			};
			
			static inline void __CalcHeight ( AddressRange * Node )
			{
				
				Node -> Height = math_max ( ( Node -> Left != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) ) ? Node -> Left -> Height : - 1, ( Node -> Right != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) ) ? Node -> Right -> Height : - 1 ) + 1;
				
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
			
			static inline Storage * __StorageFromNode ( AddressRange * Node )
			{
				
				return reinterpret_cast <Storage *> ( reinterpret_cast <uint32_t> ( Node ) & 0xFFFFF000 );
				
			};
			
			static inline uint32_t __SlotFromNode ( AddressRange * Node )
			{
				
				return ( ( ( reinterpret_cast <uint32_t> ( Node ) & 0x00000FFF ) - 16 ) / 32 );
				
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
