#include <mm/paging/PAlloc.h>
#include <mm/paging/PageTable.h>

const char * MM::Paging::PAlloc :: ErrorStrings [] = 
{
	
	"None",	
	
};

const char * MM::Paging::PAlloc :: GetErrorString ( uint32_t Error )
{
	
	if ( Error > kMaxErrorNumber )
		return "";
	
	return ErrorStrings [ Error ];
	
};

void MM::Paging::PAlloc :: InitFreeZone ( PageFreeZone ** Zone, const char * Name, uint32_t InitialPhysicalBase, uint32_t InitialPhysicalLength, uint32_t * Error )
{
	
	FreePageZone * NewZone = reinterpret_cast <FreePageZone *> ( Zone );
	
	InitialPhysicalLength -= ( ( InitialPhysicalBase + 0xFFF ) & 0xFFF ) - InitialPhysicalBase;
	InitialPhysicalBase = ( ( InitialPhysicalBase + 0xFFF ) & 0xFFF );
	
	Storage * InitialStorage = reinterpret_cast <Storage *> ( InitialPhysicalBase );
	InitialPhysicalBase += 0x1000;
	InitialPhysicalLength -= 0x1000;
	
	FreePageZone * FreeZone = reinterpret_cast <FreePageZone *> ( & InitialStorage -> Ranges [ 0 ] );
	AddressRange * InitialRange = reinterpret_cast <AddressRange *> ( & InitialStorage -> Ranges [ 1 ] );
	
	InitialStorage -> Bitmap [ 0 ] = 0x00000003;
	InitialStorage -> Bitmap [ 1 ] = 0x00000000;
	InitialStorage -> Bitmap [ 2 ] = 0x00000000;
	InitialStorage -> Bitmap [ 3 ] = 0x00000000;
	
	InitialStorage -> FreeCount = 125;
	
	InitialRange -> Base = InitialPhysicalBase;
	InitialRange -> Length = InitialPhysicalLength;
	
	InitialRange -> Parent = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	InitialRange -> Left = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	InitialRange -> Right = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	InitialRange -> PreviousInSizeClass = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	InitialRange -> NextInSizeClass = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	
	InsertNode ( & NewZone -> FreeTreeRoot, InitialRange );
	InsertNodeInSizeClass ( NewZone, InitialRange, __CalculateSizeClass ( InitialPhysicalLength ) );
	
};

MM::Paging::PAlloc :: PageAllocZone * MM::Paging::PAlloc :: MakePageAllocationZone ( const char * Name, PageFreeZone * FreeZone, uint32_t * Error )
{
	
	
	
};

uint32_t MM::Paging::PAlloc :: PrePagingCalculateVirtualSize ( PageFreeZone * Zone, uint32_t * Error )
{
	
	uint32_t PageCount = 0;
	
	Storage * CurrentStorage = reinterpret_cast <FreePageZone *> ( Zone ) -> FreeStorageHead;
	
	while ( CurrentStorage != reinterpret_cast <Storage *>  ( kStoragePTR_Invalid ) )
	{
		
		PageCount ++;
		
		CurrentStorage = CurrentStorage -> Next;
		
	}
	
	CurrentStorage = reinterpret_cast <FreePageZone *> ( Zone ) -> FullStorageHead;
	
	while ( CurrentStorage != reinterpret_cast <Storage *> ( kStoragePTR_Invalid ) )
	{
		
		PageCount ++;
		
		CurrentStorage = CurrentStorage -> Next;
		
	}
	
	return PageCount << 12;
	
};

void MM::Paging::PAlloc :: VirtualizeZone ( PageFreeZone * Zone, PageAllocZone * AllocationZones [], uint32_t AllocationZoneCount, uint32_t VirtualBase, uint32_t * Error )
{
	
	Storage * CurrentStorage = reinterpret_cast <FreePageZone *> ( Zone ) -> FreeStorageHead;
	PageAllocZone * NewZonePTRs [ AllocationZoneCount ];
	
	// TODO: virtualize trees
	
	uint32_t I;
	
	for ( I = 0; I < AllocationZoneCount; I ++ )
		NewZonePTRs [ I ] = reinterpret_cast <PageAllocZone *> ( kPageFreeZonePTR_Invalid );
	
	uint32_t Virt = VirtualBase;
	
	while ( CurrentStorage != reinterpret_cast <Storage *> ( kStoragePTR_Invalid ) )
	{
		
		for ( I = 0; I < AllocationZoneCount; I ++ )
		{
			
			if ( ( NewZonePTRs [ I ] == reinterpret_cast <PageAllocZone *> ( kPageFreeZonePTR_Invalid ) ) && ( reinterpret_cast <uint32_t> ( AllocationZones [ I ] ) > reinterpret_cast <uint32_t> ( CurrentStorage ) ) && ( reinterpret_cast <uint32_t> ( AllocationZones [ I ] ) < ( reinterpret_cast <uint32_t> ( CurrentStorage ) + sizeof ( CurrentStorage ) ) ) )
				NewZonePTRs [ I ] = reinterpret_cast <void *> ( ( reinterpret_cast <uint32_t> ( AllocationZones [ I ] ) & 0xFFF ) | Virt );
			
		}
		
		PageTable :: SetKernelMapping ( Virt, reinterpret_cast <uint32_t> ( CurrentStorage ), PageTable :: Flags_Writeable | PageTable :: Flags_Present | PageTable :: Flags_Cutsom_KMap );
		
		CurrentStorage = reinterpret_cast <Storage *> ( Virt );
		
		CurrentStorage = CurrentStorage -> Next;
		
		Virt += 0x1000;
		
	}
	
	CurrentStorage = reinterpret_cast <FreePageZone *> ( Zone ) -> FullStorageHead;
	
	while ( CurrentStorage != reinterpret_cast <Storage *> ( kStoragePTR_Invalid ) )
	{
		
		Virt = VirtualBase;
		
		for ( I = 0; I < AllocationZoneCount; I ++ )
		{
			
			if ( ( NewZonePTRs [ I ] == reinterpret_cast <PageAllocZone *> ( kPageFreeZonePTR_Invalid ) ) && ( reinterpret_cast <uint32_t> ( AllocationZones [ I ] ) > reinterpret_cast <uint32_t> ( CurrentStorage ) ) && ( reinterpret_cast <uint32_t> ( AllocationZones [ I ] ) < ( reinterpret_cast <uint32_t> ( CurrentStorage ) + sizeof ( CurrentStorage ) ) ) )
				NewZonePTRs [ I ] = reinterpret_cast <void *> ( ( reinterpret_cast <uint32_t> ( AllocationZones [ I ] ) & 0xFFF ) | Virt );
			
		}
		
		PageTable :: SetKernelMapping ( Virt, reinterpret_cast <uint32_t> ( CurrentStorage ), PageTable :: Flags_Writeable | PageTable :: Flags_Present | PageTable :: Flags_Cutsom_KMap );
		
		CurrentStorage = reinterpret_cast <Storage *> ( Virt );
		
		CurrentStorage = CurrentStorage -> Next;
		
	}
	
	for ( I = 0; I < AllocationZoneCount; I ++ )
		AllocationZones [ I ] = NewZonePTRs [ I ];
	
	Virt += 0x1000;
	
};

MM::Paging::PAlloc :: AddressRange * MM::Paging::PAlloc :: NewRange ( FreePageZone * Zone )
{
	
	Storage * FromStorage = Zone -> FreeStorageHead;
	
	uint32_t Slot = __GetFirstFreeStorageSlot ( FromStorage );
	
	FromStorage -> FreeCount --;
	
	if ( FromStorage -> FreeCount == 0 )
	{
		
		Zone -> FreeStorageHead = FromStorage -> Next;
		Zone -> FreeStorageHead -> Previous = reinterpret_cast <Storage *> ( kStoragePTR_Invalid );
		
		FromStorage -> Next = Zone -> FullStorageHead;
		Zone -> FullStorageHead -> Previous = FromStorage;
		Zone -> FullStorageHead = FromStorage;
		
	}
	
	__MarkStorageSlotUsed ( FromStorage, Slot );
	
	return & FromStorage -> Ranges [ Slot ];
	
};

void MM::Paging::PAlloc :: FreeRange ( FreePageZone * Zone, AddressRange * Range )
{
	
	
	
};

void MM::Paging::PAlloc :: RemoveNodeFromSizeClass ( FreePageZone * Zone, AddressRange * ToRemove, uint32_t SizeClass )
{
	
	if ( reinterpret_cast <FreePageZone *> ( Zone ) -> FreeSizeClassHeads [ SizeClass ] == ToRemove )
		reinterpret_cast <FreePageZone *> ( Zone ) -> FreeSizeClassHeads [ SizeClass ] = ToRemove -> NextInSizeClass;
	else if ( ToRemove -> PreviousInSizeClass != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
		ToRemove -> PreviousInSizeClass -> NextInSizeClass = ToRemove -> NextInSizeClass;
	
	if ( ToRemove -> NextInSizeClass != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
		ToRemove -> NextInSizeClass -> PreviousInSizeClass = ToRemove -> PreviousInSizeClass;
	
};

void MM::Paging::PAlloc :: InsertNodeInSizeClass ( FreePageZone * Zone, AddressRange * ToInsert, uint32_t SizeClass )
{
	
	ToInsert -> NextInSizeClass = reinterpret_cast <FreePageZone *> ( Zone ) -> FreeSizeClassHeads [ SizeClass ];
	reinterpret_cast <FreePageZone *> ( Zone ) -> FreeSizeClassHeads [ SizeClass ] = ToInsert;
	
	if ( ToInsert -> NextInSizeClass != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
		ToInsert -> NextInSizeClass -> PreviousInSizeClass = ToInsert;	
	
};

void MM::Paging::PAlloc :: InsertNode ( AddressRange ** Root, AddressRange * ToInsert )
{
	
	ToInsert -> Height = 0;
	
	if ( ( * Root ) == reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
	{
		
		( * Root ) = ToInsert;
		
		return;
		
	}
	
	AddressRange * Current = ( * Root );
	
	while ( true )
	{
		
		if ( ToInsert -> Base > Current -> Base )
		{
			
			if ( Current -> Right == reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
			{
				
				ToInsert -> Parent = Current;
				Current -> Right = ToInsert;
				
				break;
				
			}
			
			Current = Current -> Right;
			
		}
		else
		{
			
			if ( Current -> Left == reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
			{
				
				ToInsert -> Parent = Current;
				Current -> Left = ToInsert;
				
				break;
				
			}
			
			Current = Current -> Left;
			
			
		}
		
	}
	
	Current = ToInsert;
	
	do
	{
		
		Current = Current -> Parent;
		__CalcHeight ( Current );
		
		if ( Current == ( * Root ) )
		{
			
			( * Root ) = Balance ( Current );
			
			break;
			
		}
		
		Current = Balance ( Current );
		
	} 
	while ( Current -> Parent != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) );
	
};

void MM::Paging::PAlloc :: RemoveNode ( AddressRange ** Root, AddressRange * ToRemove )
{
	
	AddressRange * UnbalancedNode;
	AddressRange * SuccessorNode;
	
	if ( ToRemove == ( * Root ) )
	{
		
		if ( ToRemove -> Left != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
		{
			
			if ( ToRemove -> Right != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
			{
				
				SuccessorNode = __LeftMostNode ( ToRemove -> Right );
				UnbalancedNode = SuccessorNode -> Parent;
				
				if ( SuccessorNode -> Right != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
				{
					
					if ( UnbalancedNode -> Right == SuccessorNode )
						UnbalancedNode -> Right = SuccessorNode -> Right;
					else
						UnbalancedNode -> Left = SuccessorNode -> Right;
					
					SuccessorNode -> Right -> Parent = UnbalancedNode;
					
				}
				else
				{
					
					if ( UnbalancedNode -> Right == SuccessorNode )
						UnbalancedNode -> Right = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
					else
						UnbalancedNode -> Left = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
					
				}
				
				if ( UnbalancedNode == ( * Root ) )
				{
					
					UnbalancedNode = SuccessorNode;
					( * Root ) -> Right = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
					
				}
				
				( * Root ) = SuccessorNode;
				SuccessorNode -> Parent = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				
				SuccessorNode -> Left = ToRemove -> Left;
				ToRemove -> Left -> Parent = SuccessorNode;
				
				SuccessorNode -> Right = ToRemove -> Right;
				if ( ToRemove -> Right != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
					ToRemove -> Right -> Parent = SuccessorNode;
				
				ToRemove -> Left = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				ToRemove -> Right = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				ToRemove -> Parent = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				
			}
			else
			{
				
				( * Root ) = ToRemove -> Left;
				ToRemove -> Left = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				
				return;
				
			}
			
		}
		else
		{
			
			if ( ToRemove -> Right != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
			{
				
				( * Root ) = ToRemove -> Right;
				ToRemove -> Right = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				
				return;
				
			}
			else
			{
				
				( * Root ) = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				
				return;
				
			}
			
		}
		
	}
	else
	{
		
		if ( ToRemove -> Left != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
		{
			
			if ( ToRemove -> Right != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
			{
				
				SuccessorNode = __LeftMostNode ( ToRemove -> Right );
				UnbalancedNode = SuccessorNode -> Parent;
				
				if ( SuccessorNode -> Right != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
				{
					
					if ( UnbalancedNode -> Left == SuccessorNode )
						UnbalancedNode -> Left = SuccessorNode -> Right;
					else
						UnbalancedNode -> Right = SuccessorNode -> Right;
					
					SuccessorNode -> Right -> Parent = SuccessorNode -> Parent;
					
				}
				else
				{
					
					if ( UnbalancedNode -> Right == SuccessorNode )
						UnbalancedNode -> Right = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
					else
						UnbalancedNode -> Left = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
					
				}
				
				if ( UnbalancedNode == ToRemove )
				{
					
					UnbalancedNode = SuccessorNode;
					ToRemove -> Right = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
					
				}
				
				if ( ToRemove -> Parent -> Left == ToRemove )
					ToRemove -> Parent -> Left = SuccessorNode;
				else
					ToRemove -> Parent -> Right = SuccessorNode;
				
				SuccessorNode -> Parent = ToRemove -> Parent;
				
				SuccessorNode -> Right = ToRemove -> Right;
				if ( SuccessorNode -> Right != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
					SuccessorNode -> Right -> Parent = SuccessorNode;
				
				SuccessorNode -> Left = ToRemove -> Left;
				SuccessorNode -> Left -> Parent = SuccessorNode;
				
				ToRemove -> Parent = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				ToRemove -> Left = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				ToRemove -> Right = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				
			}
			else
			{
				
				if ( ToRemove -> Parent -> Right == ToRemove )
					ToRemove -> Parent -> Right = ToRemove -> Left;
				else
					ToRemove -> Parent -> Left = ToRemove -> Left;
				
				UnbalancedNode = ToRemove -> Left -> Parent = ToRemove -> Parent;
				
				ToRemove -> Parent = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				ToRemove -> Left = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				
			}
			
		}
		else
		{
			
			if ( ToRemove -> Right != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
			{
				
				if ( ToRemove -> Parent -> Right == ToRemove )
					ToRemove -> Parent -> Right = ToRemove -> Right;
				else
					ToRemove -> Parent -> Left = ToRemove -> Right;
				
				UnbalancedNode = ToRemove -> Right -> Parent = ToRemove -> Parent;
				ToRemove -> Parent = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				ToRemove -> Right = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				
			}
			else
			{
				
				if ( ToRemove -> Parent -> Right == ToRemove )
					ToRemove -> Parent -> Right = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				else
					ToRemove -> Parent -> Left = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				
				UnbalancedNode = ToRemove -> Parent;
				ToRemove -> Parent = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				
			}
			
		}
		
	}
	
	while ( true )
	{
		
		__CalcHeight ( UnbalancedNode );
		
		if ( UnbalancedNode == ( * Root ) )
		{
			
			( * Root ) = Balance ( UnbalancedNode );
			
			return;
			
		}
		
		UnbalancedNode = Balance ( UnbalancedNode );
		UnbalancedNode = UnbalancedNode -> Parent;
		
	}
	
};

MM::Paging::PAlloc :: AddressRange * MM::Paging::PAlloc :: Balance ( AddressRange * LocalRoot )
{
	
	int32_t Balance = 0;
	int32_t SubBalance = 0;
	
	Balance += ( LocalRoot -> Left != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) ) ? LocalRoot -> Left -> Height : - 1;
	Balance -= ( LocalRoot -> Right != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) ) ? LocalRoot -> Right -> Height : - 1;
	
	if ( Balance > 1 )
	{
		
		SubBalance += ( LocalRoot -> Left -> Left != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) ) ? LocalRoot -> Left -> Left -> Height : - 1;
		SubBalance -= ( LocalRoot -> Left -> Right != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) ) ? LocalRoot -> Left -> Right -> Height : - 1;
		
		if ( SubBalance > 0 )
		{
			
			LocalRoot = __RotateRight ( LocalRoot );
			
		}
		else
		{
			
			__RotateLeft ( LocalRoot -> Left );
			LocalRoot = __RotateRight ( LocalRoot );
			
		}
		
	}
	else if ( Balance < - 1 )
	{
		
		SubBalance -= ( LocalRoot -> Right -> Left != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) ) ? LocalRoot -> Right -> Left -> Height : - 1;
		SubBalance += ( LocalRoot -> Right -> Right != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) ) ? LocalRoot -> Right -> Right -> Height : - 1;
		
		if ( SubBalance > 0 )
		{
			
			LocalRoot = __RotateLeft ( LocalRoot );
			
		}
		else
		{
			
			__RotateRight ( LocalRoot -> Right );
			LocalRoot = __RotateLeft ( LocalRoot );
			
		}
		
	}
	
	return LocalRoot;
	
};

MM::Paging::PAlloc :: AddressRange * MM::Paging::PAlloc :: FindBest ( FreePageZone * Zone, uint32_t MinimumSizeClass )
{
	
	while ( MinimumSizeClass <= 19 )
	{
		
		if ( Zone -> FreeSizeClassHeads [ MinimumSizeClass ] != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
			return Zone -> FreeSizeClassHeads [ MinimumSizeClass ];
		
		MinimumSizeClass ++;
		
	}
	
	return reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	
};

MM::Paging::PAlloc :: AddressRange * MM::Paging::PAlloc :: FindNode ( AddressRange * Root, uint32_t Base )
{
	
	AddressRange * Node = Root;
	
	while ( true )
	{
		
		if ( Node == reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
			return reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
		
		if ( Node -> Base == Base )
			return Node;
		
		if ( Node -> Base > Base )
			Node = Node -> Left;
		else if ( Node -> Base < Base )
			Node = Node -> Right;
		
	}
	
};

MM::Paging::PAlloc :: AddressRange * MM::Paging::PAlloc :: FindClosestSmallerNode ( AddressRange * Root, AddressRange * Node )
{
	
	AddressRange * SearchNode = Root;
	
	if ( SearchNode == reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
		return SearchNode;
	
	while ( true )
	{
		
		if ( SearchNode -> Base >= Node -> Base )
		{
			
			if ( SearchNode -> Left == reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
				return SearchNode;
			
			SearchNode = SearchNode -> Left;
			
		}
		else
		{
			
			if ( SearchNode -> Right == reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
				return SearchNode;
			
			SearchNode = SearchNode -> Right;
			
		}
		
	}
	
};
