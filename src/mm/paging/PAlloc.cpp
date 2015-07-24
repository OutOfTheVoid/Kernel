#include <mm/paging/PAlloc.h>
#include <mm/paging/PageTable.h>
#include <mm/paging/AddressSpace.h>

const char * MM::Paging::PAlloc :: ErrorStrings [] = 
{
	
	"None",	
	"Out of physical memory",
	"Out of virtual memory",
	"Memory freed was not allocated"
	
};

const char * MM::Paging::PAlloc :: GetErrorString ( uint32_t Error )
{
	
	if ( Error > kMaxErrorNumber )
		return "";
	
	return ErrorStrings [ Error ];
	
};

void MM::Paging::PAlloc :: InitFreeZone ( PageFreeZone ** Zone, const char * Name, uint32_t InitialPhysicalBase, uint32_t InitialPhysicalLength, uint32_t InitialVPage, uint32_t * Error )
{
	
	FreePageZone ** NewZone = reinterpret_cast <FreePageZone **> ( Zone );
	
	InitialPhysicalLength -= ( ( InitialPhysicalBase + 0xFFF ) & 0xFFFFF000 ) - InitialPhysicalBase;
	InitialPhysicalBase = ( InitialPhysicalBase + 0xFFF ) & 0xFFFFF000;
	InitialPhysicalLength = ( InitialPhysicalLength + 0xFFF ) & 0xFFFFF000;
	
	if ( InitialPhysicalLength < 0x2000 )
	{
		
		* Error = kInitFreeZone_Error_OutOfPhysicalMemroy;
		
		return;
		
	}
	
	MM::Paging::PageTable :: SetKernelMapping ( InitialVPage, InitialPhysicalBase, MM::Paging::PageTable :: Flags_Present | MM::Paging::PageTable :: Flags_Writeable | MM::Paging::PageTable :: Flags_Cutsom_KMap );
	
	InitialPhysicalBase += 0x1000;
	InitialPhysicalLength -= 0x1000;
	
	Storage * InitialStorage = reinterpret_cast <Storage *> ( InitialVPage );
	
	* NewZone = reinterpret_cast <FreePageZone *> ( & InitialStorage -> Ranges [ 0 ] );
	AddressRange * InitialRange = reinterpret_cast <AddressRange *> ( & InitialStorage -> Ranges [ 7 ] );
	AddressRange * InitialStorageRange = reinterpret_cast <AddressRange *> ( & InitialStorage -> Ranges [ 8 ] );
	
	InitialStorageRange -> Base = InitialPhysicalBase - 0x1000;
	InitialStorageRange -> Length = 0x1000;
	
	InitialStorageRange -> Parent = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	InitialStorageRange -> Left = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	InitialStorageRange -> Right = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	InitialStorageRange -> PreviousInSizeClass = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	InitialStorageRange -> NextInSizeClass = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	
	InitialStorage -> Bitmap [ 0 ] = 0x000001FF;
	InitialStorage -> Bitmap [ 1 ] = 0x00000000;
	InitialStorage -> Bitmap [ 2 ] = 0x00000000;
	InitialStorage -> Bitmap [ 3 ] = 0x00000000;
	InitialStorage -> FreeCount = 117;
	
	InitialRange -> Base = InitialPhysicalBase;
	InitialRange -> Length = InitialPhysicalLength;
	
	InitialRange -> Parent = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	InitialRange -> Left = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	InitialRange -> Right = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	InitialRange -> PreviousInSizeClass = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	InitialRange -> NextInSizeClass = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	
	( * NewZone ) -> FreePageCount = InitialPhysicalLength >> 12;
	( * NewZone ) -> FreeStorageSlotCount = InitialStorage -> FreeCount;
	( * NewZone ) -> FreeStorageHead = InitialStorage;
	( * NewZone ) -> FullStorageHead = reinterpret_cast <Storage *> ( kStoragePTR_Invalid );
	
	( * NewZone ) -> StorageTreeRoot = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	( * NewZone ) -> FreeTreeRoot = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	
	for ( uint32_t I = 0; I < 20; I ++ )
		( * NewZone ) -> FreeSizeClassHeads [ I ] = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	
	uint32_t InitSizeClass = __CalculateSizeClass ( InitialPhysicalLength );
	
	InsertNodeInSizeClass ( ( * NewZone ), InitialRange, InitSizeClass );
	InsertNode ( & ( * NewZone ) -> FreeTreeRoot, InitialRange );
	
	InsertNode ( & ( * NewZone ) -> StorageTreeRoot, InitialStorageRange );
	
	( * NewZone ) -> Name = Name;
	
	* Error = kError_None;
	
};

MM::Paging::PAlloc :: PageAllocZone * MM::Paging::PAlloc :: MakePageAllocationZone ( const char * Name, PageFreeZone * FreeZone, uint32_t * Error )
{
	
	FreePageZone * Zone = reinterpret_cast <FreePageZone *> ( FreeZone );
	AddressRange * NewSpace = NewRange ( Zone );
	
	PageAllocationZone * NewZone = reinterpret_cast <PageAllocationZone *> ( NewSpace );
	
	if ( ! ExpandStorage ( Zone ) )
	{
		
		FreeRange ( Zone, NewSpace );
		
		* Error = kMakePageAllocation_Error_OutOfPhysicalMemroy;
		
		return NULL;
		
	}
	
	NewZone -> Name = Name;
	NewZone -> FreeZone = Zone;
	NewZone -> UsedTreeRoot = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	NewZone -> AllocatedPageCount = 0;
	
	* Error = kError_None;
	
	return NewZone;
	
};

uint32_t MM::Paging::PAlloc :: GetAllocationSize ( PageAllocZone * AllocationZone, void * Address )
{
	
	PageAllocationZone * Zone = reinterpret_cast <PageAllocationZone *> ( AllocationZone );
	
	AddressRange * Range = FindNode ( Zone -> UsedTreeRoot, reinterpret_cast <uint32_t> ( Address ) );
	
	if ( Range == reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
		return 0;
	
	return Range -> Length;
	
};

uint32_t MM::Paging::PAlloc :: GetFreePages ( PageFreeZone * FreeZone )
{
	
	FreePageZone * Zone = reinterpret_cast <FreePageZone *> ( FreeZone );
	
	if ( Zone == NULL )
		return 0;
	
	return Zone -> FreePageCount;
	
};

void MM::Paging::PAlloc :: Alloc ( PageAllocZone * AllocationZone, void ** Address, uint32_t Length, uint32_t * Error )
{
	
	if ( Length == 0 )
	{
		
		* Error = kError_None;
		
		return;
		
	}
	
	PageAllocationZone * Zone = reinterpret_cast <PageAllocationZone *> ( AllocationZone );
	
	AddressRange * AllocationRange = DoAlloc ( Zone -> FreeZone, Length );
	
	if ( AllocationRange == reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
	{
		
		* Error = kMakePageAllocation_Error_OutOfPhysicalMemroy;
		
		return;
		
	}
	
	InsertNode ( & Zone -> UsedTreeRoot, AllocationRange );
	
	if ( ! ExpandStorage ( Zone -> FreeZone ) )
	{
		
		Free ( AllocationZone, reinterpret_cast <void *> ( AllocationRange -> Base ), Error );
		
		return;
		
	}
	
	Zone -> AllocatedPageCount += Length >> 12;
	* Address = reinterpret_cast <void *> ( AllocationRange -> Base );
	
	* Error = kError_None;
	
};

void MM::Paging::PAlloc :: Free ( PageAllocZone * AllocationZone, void * Address, uint32_t * Error )
{
	
	PageAllocationZone * Zone = reinterpret_cast <PageAllocationZone *> ( AllocationZone );
	
	AddressRange * Range = FindNode ( Zone -> UsedTreeRoot, reinterpret_cast <uint32_t> ( Address ) );
	
	if ( Range == reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
	{
		
		* Error = kError_Free_NotAllocated;
		
		return;
		
	}
	
	RemoveNode ( & Zone -> UsedTreeRoot, Range );
	Zone -> AllocatedPageCount -= Range -> Length >> 12;
	
	DoFree ( Zone -> FreeZone, Range );
	
	* Error = kError_None;
	
};

void MM::Paging::PAlloc :: DoFree ( FreePageZone * Zone, AddressRange * Range )
{
	
	if ( Range == reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
		return;
	
	uint32_t SizeClass = __CalculateSizeClass ( Range -> Length );
	
	Zone -> FreePageCount += Range -> Length >> 12;
	
	AddressRange * Lower = FindClosestSmallerNode ( Zone -> FreeTreeRoot, Range );
	AddressRange * Higher = FindNode ( Zone -> FreeTreeRoot, Range -> Length + Range -> Base );
	
	if ( Higher != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
	{
		
		RemoveNodeFromSizeClass ( Zone, Range, SizeClass );
		RemoveNodeFromSizeClass ( Zone, Higher, __CalculateSizeClass ( Higher -> Length ) );
		RemoveNode ( & Zone -> FreeTreeRoot, Higher );
		
		Range -> Length += Higher -> Length;
		SizeClass = __CalculateSizeClass ( Range -> Length );
		
		FreeRange ( Zone, Higher );
		
		}
	
	if ( Lower != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
	{
		
		if ( ( Lower != Range ) && ( ( Lower -> Base + Lower -> Length ) == Range -> Base ) )
		{
			
			RemoveNodeFromSizeClass ( Zone, Range, SizeClass );
			
			
			uint32_t LowerSizeClass = __CalculateSizeClass ( Lower -> Length );
			
			Lower -> Length += Range -> Length;
			
			uint32_t LowerSizeClassUpdated = __CalculateSizeClass ( Lower -> Length );
			
			if ( LowerSizeClass != LowerSizeClassUpdated )	
			{
				
				RemoveNodeFromSizeClass ( Zone, Lower, LowerSizeClass );
				InsertNodeInSizeClass ( Zone, Lower, LowerSizeClassUpdated );
				
				
			}
			
			FreeRange ( Zone, Range );
			
			
			return;
			
		}
			
	}
	
	InsertNodeInSizeClass ( Zone, Range, SizeClass );
	InsertNode ( & Zone -> FreeTreeRoot, Range );
	
	};

MM::Paging::PAlloc :: AddressRange * MM::Paging::PAlloc :: DoAlloc ( FreePageZone * Zone, uint32_t Length )
{
	
	uint32_t SizeClass = __CalculateSizeClass ( Length );
	
	AddressRange * BorrowRange = FindBest ( Zone, __CalculateSizeClass ( Length ) );
		
	if ( BorrowRange == reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
		return reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	
	if ( BorrowRange -> Length == Length )
	{
		
		RemoveNodeFromSizeClass ( Zone, BorrowRange, SizeClass );
		
		RemoveNode ( & Zone -> FreeTreeRoot, BorrowRange );
		
		Zone -> FreePageCount -= Length >> 12;
		
		return BorrowRange;
		
	};
	
	AddressRange * AllocRange = NewRange ( Zone );
	
	if ( AllocRange == reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
		return reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	
	AllocRange -> Parent = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	AllocRange -> Left = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	AllocRange -> Right = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	AllocRange -> NextInSizeClass = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	AllocRange -> PreviousInSizeClass = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	
	uint32_t OldSizeClass = __CalculateSizeClass ( BorrowRange -> Length );
	
	BorrowRange -> Length -= Length;
	AllocRange -> Base = BorrowRange -> Base + BorrowRange -> Length;
	AllocRange -> Length = Length;
	
	uint32_t NewSizeClass = __CalculateSizeClass ( BorrowRange -> Length );
	
	if ( NewSizeClass != OldSizeClass )
	{
		
		RemoveNodeFromSizeClass ( Zone, BorrowRange, OldSizeClass );
		InsertNodeInSizeClass ( Zone, BorrowRange, NewSizeClass );
		
	}
	
	Zone -> FreePageCount -= Length >> 12;
	
	return AllocRange;
	
};

bool MM::Paging::PAlloc :: ExpandStorage ( FreePageZone * Zone )
{
	
	if ( Zone -> FreeStorageSlotCount <= 3 )
	{
		
		MM::Paging::AddressSpace * KernelAddressSpace = MM::Paging::AddressSpace :: RetrieveKernelAddressSpace ();
		MM::Paging::AddressSpace :: AddressRange * AllocationRange = KernelAddressSpace -> DoAlloc ( 0x1000 );
		
		if ( AllocationRange == reinterpret_cast <MM::Paging::AddressSpace :: AddressRange *> ( MM::Paging::AddressSpace :: kAddressRangePTR_Invalid ) )
			return false;
		
		AddressRange * AllocationRangePhysical = DoAlloc ( Zone, 0x1000 );
		
		if ( AllocationRangePhysical == reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
		{
			
			uint32_t DummyError;
			KernelAddressSpace -> Free ( reinterpret_cast <void *> ( AllocationRange -> Base ), & DummyError );
			
			return false;
			
		}
		
		InsertNode ( & Zone -> StorageTreeRoot, AllocationRangePhysical );
		
		MM::Paging::PageTable :: SetKernelMapping ( AllocationRange -> Base, AllocationRangePhysical -> Base, MM::Paging::PageTable :: Flags_Present | MM::Paging::PageTable :: Flags_Writeable | MM::Paging::PageTable :: Flags_Cutsom_KMap );
		
		Storage * NewStorage = reinterpret_cast <Storage *> ( AllocationRange -> Base );
		
		NewStorage -> Bitmap [ 0 ] = 0x00000000;
		NewStorage -> Bitmap [ 1 ] = 0x00000000;
		NewStorage -> Bitmap [ 2 ] = 0x00000000;
		NewStorage -> Bitmap [ 3 ] = 0x00000000;
		NewStorage -> FreeCount = 127;
		
		NewStorage -> Next = Zone -> FreeStorageHead;
		
		if ( Zone -> FreeStorageHead != reinterpret_cast <Storage *> ( kStoragePTR_Invalid ) )
			Zone -> FreeStorageHead -> Previous = NewStorage;
		
		Zone -> FreeStorageHead = NewStorage;
		
		Zone -> FreeStorageSlotCount += 127;
		
		uint32_t DummyError;
		KernelAddressSpace -> CheckStorage ( & DummyError );
		
		if ( DummyError != MM::Paging::AddressSpace :: kCreateAddressSpace_Error_None )
		{
			
			Zone -> FreeStorageHead = NewStorage -> Next;
			
			if ( NewStorage -> Next != reinterpret_cast <Storage *> ( kStoragePTR_Invalid ) )
				NewStorage -> Next -> Previous = reinterpret_cast <Storage *> ( kStoragePTR_Invalid );
			
			Zone -> FreeStorageSlotCount -= 127;
			
			MM::Paging::PageTable :: SetKernelMapping ( AllocationRange -> Base, AllocationRangePhysical -> Base, MM::Paging::PageTable :: Flags_Writeable );
			
			uint32_t DummyError;
			KernelAddressSpace -> Free ( reinterpret_cast <void *> ( AllocationRange -> Base ), & DummyError );
			
			DoFree ( Zone, AllocationRangePhysical );
			
			return false;
			
		}
		
	}
	
	return true;
	
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
	
	// TODO: Handle watermark underflow
	
};

void MM::Paging::PAlloc :: FreeRange ( FreePageZone * Zone, AddressRange * Range )
{
	
	Storage * ToStorage = __StorageFromNode ( Range );
	uint32_t Slot = __SlotFromNode ( Range );
	
	 __MarkStorageSlotClear ( ToStorage, Slot );
	 
	ToStorage -> FreeCount ++;
	
	if ( ToStorage -> FreeCount == 1 )
	{
		
		if ( Zone -> FullStorageHead == ToStorage )
		{
			
			Zone -> FullStorageHead = ToStorage -> Next;
			
			if ( Zone -> FullStorageHead != reinterpret_cast <Storage *> ( kStoragePTR_Invalid ) )
				Zone -> FullStorageHead -> Previous = reinterpret_cast <Storage *> ( kStoragePTR_Invalid );
			
		}
		else
		{
			
			ToStorage -> Previous -> Next = reinterpret_cast <Storage *> ( kStoragePTR_Invalid );
			
			if ( ToStorage -> Next != reinterpret_cast <Storage *> ( kStoragePTR_Invalid ) )
				ToStorage -> Next -> Previous = ToStorage -> Previous;
			
		}
		
		ToStorage -> Next = Zone -> FreeStorageHead;
		
		if ( Zone -> FreeStorageHead != reinterpret_cast <Storage *> ( kStoragePTR_Invalid ) )
			Zone -> FreeStorageHead -> Previous = ToStorage;
		
		Zone -> FreeStorageHead = ToStorage;
		
	}
	
	// TODO: Handle watermark overflow...
	
};

void MM::Paging::PAlloc :: RemoveNodeFromSizeClass ( FreePageZone * Zone, AddressRange * ToRemove, uint32_t SizeClass )
{
	
	if ( Zone -> FreeSizeClassHeads [ SizeClass ] == ToRemove )
		Zone -> FreeSizeClassHeads [ SizeClass ] = ToRemove -> NextInSizeClass;
	else if ( ToRemove -> PreviousInSizeClass != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
		ToRemove -> PreviousInSizeClass -> NextInSizeClass = ToRemove -> NextInSizeClass;
	
	if ( ToRemove -> NextInSizeClass != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
		ToRemove -> NextInSizeClass -> PreviousInSizeClass = ToRemove -> PreviousInSizeClass;
	
	ToRemove -> NextInSizeClass = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	
	ToRemove -> PreviousInSizeClass = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	
};

void MM::Paging::PAlloc :: InsertNodeInSizeClass ( FreePageZone * Zone, AddressRange * ToInsert, uint32_t SizeClass )
{
	
	ToInsert -> NextInSizeClass = Zone -> FreeSizeClassHeads [ SizeClass ];
	Zone -> FreeSizeClassHeads [ SizeClass ] = ToInsert;
	
	if ( ToInsert -> NextInSizeClass != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
		ToInsert -> NextInSizeClass -> PreviousInSizeClass = ToInsert;
	
	ToInsert -> PreviousInSizeClass = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	
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
				( * Root ) -> Parent = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				ToRemove -> Left = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				
				return;
				
			}
			
		}
		else
		{
			
			if ( ToRemove -> Right != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
			{
				
				( * Root ) = ToRemove -> Right;
				( * Root ) -> Parent = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
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
	
	while ( MinimumSizeClass < 20 )
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

void MM::Paging::PAlloc :: AddFreePageRange ( PageFreeZone * FreeZone, uint32_t Base, uint32_t Length, uint32_t * Error )
{
	
	FreePageZone * Zone = reinterpret_cast <FreePageZone *> ( FreeZone );
	
	Length -= ( ( 0xFFF + Base ) & 0xFFFFF000 ) - Base;
	Base = ( ( 0xFFF + Base ) & 0xFFFFF000 );
	Length &= 0xFFFFF000;
	
	if ( Length == 0 )
		return;
	
	if ( ! ExpandStorage ( Zone ) )
	{
		
		* Error = kAddFreePageRange_Error_OutOfPhysicalMemroy;
		
		return;
		
	}
	
	AddressRange * Range = NewRange ( Zone );
	
	if ( Range == reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
	{
		
		* Error = kAddFreePageRange_Error_OutOfPhysicalMemroy;
		
		return;
		
	}
	
	Range -> Parent = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	Range -> Left = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	Range -> Right = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	Range -> PreviousInSizeClass = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	Range -> NextInSizeClass = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	Range -> Base = Base;
	Range -> Length = Length;
	
	uint32_t SizeClass = __CalculateSizeClass ( Length );
	
	InsertNodeInSizeClass ( Zone, Range, SizeClass );
	InsertNode ( & Zone -> FreeTreeRoot, Range );
	
	Zone -> FreePageCount += Length >> 12;
	
	* Error = kError_None;
	
};
