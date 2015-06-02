#ifndef MM_PAGING_PFA_H
#define MM_PAGING_PFA_H

#include <stdint.h>
#include <stddef.h>

#include <boot/multiboot/multiboot.h>

#include <mm/MM.h>
#include <mm/paging/Paging.h>
#include <mm/paging/PageTable.h>
#include <mm/paging/AddressSpace.h>

#include <system/func/kprintf.h>
#include <hw/video/VText.h>

#include <math/bitmath.h>

#define PFA_DEBUG

namespace MM
{
	
	namespace Paging
	{
		
		class PFA
		{
		public:
			
			static void Init ( multiboot_info_t * MultibootInfo );
			static multiboot_info_t * RetrieveMultibootInfoCopy ();
			
			static bool Alloc ( uint32_t Length, void ** Address );
			static void Free ( void * Address );
			
			static uint32_t GetAllocationSize ( void * Address );
			
			static uint32_t GetFreeKB ();
			static uint32_t GetTotalKB ();
			
		private:
			
			friend class PageTable;
			friend class AddressSpace;
			
			typedef struct
			{
				
				uint8_t AFree:1;
				uint8_t BFree:1;
				uint8_t Split:1;
				uint8_t Usable:1;
				
				uint8_t Unused:4;
				
			} __attribute__ ((packed)) Entry;
			
			static Entry * Table;
			static size_t TableSize;
			static uint32_t TopLevel;
			static uint32_t FreeCount;
			static uint32_t TotalCount;
			
			static size_t CalcMBICopySize ( multiboot_info_t * MultibootInfo );
			static void CopyMBI ( multiboot_info_t * Old, multiboot_info_t * New );
			static void MBEarlyAnalyze ( multiboot_info_t * MultibootInfo, uint32_t * MaxAddress );
			static bool TestRegionUsable ( uint32_t Offset, uint32_t Length );
			
			static multiboot_info_t * MBICopy;
			static size_t MBICopySize;
			
			static inline uint32_t __CalcTreeIndex ( uint32_t Addr, uint32_t Level )
			{
				
				return ( ( ( Addr >> 12 ) >> Level ) + ( 1 << ( TopLevel - Level ) ) - 1 );
				
			};
			
			static inline uint32_t __CalcChildAIndex ( uint32_t ParentIndex )
			{
				
				return ( ParentIndex << 1 ) + 1;
				
			};
			
			static inline uint32_t __CalcChildBIndex ( uint32_t ParentIndex )
			{
				
				return ( ParentIndex + 1 ) << 1;
				
			};
			
			static inline uint32_t __CalcParentIndex ( uint32_t ChildIndex )
			{
				
				return ( ( ChildIndex - 1 ) & ~ 0x1 ) >> 1;
				
			};
			
			static inline uint32_t __FindFree ( uint32_t Index, uint32_t TargetLevel, uint32_t Level )
			{
				
				Entry This = Table [ Index ];
				
				if ( Level == TargetLevel )
				{
					
					if ( ( This.AFree & This.Usable ) && ( This.Split == 0 ) )
						return Index;
					
					return 0xFFFFFFFF;
					
				}
				
				if ( This.AFree )
				{
					
					uint32_t Temp = __FindFree (  __CalcChildAIndex ( Index ), TargetLevel, Level - 1 );
					
					if ( Temp != 0xFFFFFFFF )
						return Temp;
					
				}
				
				if ( This.BFree )
					return __FindFree ( __CalcChildBIndex ( Index ), TargetLevel, Level - 1 );
				
				return 0xFFFFFFFF;
				
			};
			
			static uint32_t __FindAllocated ( uint32_t Address, uint32_t Level, uint32_t * FoundLevel )
			{
				
				uint32_t Index = __CalcTreeIndex ( Address, Level );
				
				if ( Table [ Index ].Usable )
				{
					
					if ( ! Table [ Index ].Split )
					{
						
						* FoundLevel = Level;
						
						return ( Table [ Index ].AFree ) ? 0xFFFFFFFF : Index;
						
					}
					
					return __FindAllocated ( Address, Level - 1, FoundLevel );
					
				}
				
				return 0xFFFFFFFF;
				
			};
			
			static inline void __CascadeChangesUp ( uint32_t Index )
			{
				
				Entry A = Table [ __CalcChildAIndex ( Index ) ];
				Entry B = Table [ __CalcChildBIndex ( Index ) ];
				
				Table [ Index ].AFree = A.Usable & ( A.AFree | A.BFree );
				Table [ Index ].BFree = B.Usable & ( B.AFree | B.BFree );
				Table [ Index ].Usable = ( A.Usable | B.Usable );
				Table [ Index ].Split = Table [ Index ].Usable & ( ! ( Table [ Index ].AFree & Table [ Index ].BFree ) );
				
				if ( Index == 0 )
					return;
				
				__CascadeChangesUp ( __CalcParentIndex ( Index ) );
				
			};
			
		};

	};

};

#endif
