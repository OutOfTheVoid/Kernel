#include <mm/paging/AddressSpace.h>

#include <mm/paging/PFA.h>
#include <mm/paging/PageTable.h>

#include <system/func/panic.h>
#include <system/func/kprintf.h>
#include <system/func/pmalloc.h>

#include <cpputil/Unused.h>

const char * MM::Paging::AddressSpace :: ErrorStrings [] = { 
	"None",
	"Initial kernel free zone was not large enough to allocate required structures.",
	"Failed to allocate physical memory.",
	"Failed to find free kernel virtual zone.",
	"Ran out of space.",
	"Allocation is unimplemented for non-kernel address spaces.",
	"The memory freed was not allocated.",
	"The memory referenced was not allocated.",
	"system_func_pmalloc failed to allocate a kernel page."
};

char MM::Paging::AddressSpace :: KernelAddressSpaceMem [ sizeof ( AddressSpace ) + 4 ];
MM::Paging::AddressSpace * MM::Paging::AddressSpace :: KernelAddressSpace = NULL;

void MM::Paging::AddressSpace :: KInit ( uint32_t KernelFreeBase, uint32_t KernelMemSize )
{
	
	uint32_t Error;
	
	KernelAddressSpace = new( reinterpret_cast <void *> ( ( reinterpret_cast <uint32_t> ( & KernelAddressSpaceMem ) + 4 ) & ~ 0x03 ) ) AddressSpace ();
	CreateAddressSpace ( KernelAddressSpace, true, KernelFreeBase, KernelMemSize, & Error );
	
	if ( Error != kCreateAddressSpace_Error_None )
		KPANIC ( "Kernel address space init failure: \"%s\"", GetErrorString ( Error ) );
	
};

const char * MM::Paging::AddressSpace :: GetErrorString ( uint32_t Error )
{
	
	if ( Error > kMaxErrorNumber )
		return "";
	
	return ErrorStrings [ Error ];
	
};

MM::Paging::AddressSpace * MM::Paging::AddressSpace :: RetrieveKernelAddressSpace ()
{
	
	return KernelAddressSpace;
	
};

void MM::Paging::AddressSpace :: CreateAddressSpace ( AddressSpace * Space, bool Kernel, uint32_t InitialFreeBase, uint32_t InitialFreeLength, uint32_t * Error )
{
	
	Storage * InitialStorage = reinterpret_cast <Storage *> ( kStoragePTR_Invalid );
	AddressRange * InitialRange;
	
	uint32_t NewInitialFreeBase = 0xFFF + InitialFreeBase;
	NewInitialFreeBase &= ~ 0xFFF;
	InitialFreeLength -= NewInitialFreeBase - InitialFreeBase;
	InitialFreeLength &= ~ 0xFFF;
	
	Space -> TotalPageCount = InitialFreeLength >> 12;
	
	if ( Kernel )
	{
		
		void * ISPhysical;
		
		if ( NewInitialFreeBase < 0x2000 )
		{
			
			* Error = kCreateAddressSpace_Error_InitialZoneSize;
			
			return;
			
		}
		
		if ( ! MM::Paging::PFA :: Alloc ( 4096, & ISPhysical ) )
		{
			
			* Error = kCreateAddressSpace_Error_FailedPhysicalAllocation;
			
			return;
			
		}
		
		MM::Paging::PageTable :: SetKernelMapping ( NewInitialFreeBase, reinterpret_cast <uint32_t> ( ISPhysical ), MM::Paging::PageTable :: Flags_Present | MM::Paging::PageTable :: Flags_Writeable );
		InitialStorage = reinterpret_cast <Storage *> ( NewInitialFreeBase );
		NewInitialFreeBase += 0x1000;
		InitialFreeLength -= 0x1000;
		
	}
	else
	{
		
		InitialStorage = reinterpret_cast <Storage *> ( system_func_pmalloc ( 1 ) );
		
		if ( InitialStorage == NULL )
		{
			
			* Error = kCreateAddressSpace_Error_FailedPMalloc;
			
			return;
			
		}
		
	}
	
	InitialStorage -> Previous = reinterpret_cast <Storage *> ( kStoragePTR_Invalid );
	InitialStorage -> Next = reinterpret_cast <Storage *> ( kStoragePTR_Invalid );
	
	InitialStorage -> Bitmap [ 0 ] = 0x00000003;
	InitialStorage -> Bitmap [ 1 ] = 0x00000000;
	InitialStorage -> Bitmap [ 2 ] = 0x00000000;
	InitialStorage -> Bitmap [ 3 ] = 0x00000000;
	InitialStorage -> FreeCount = 111;
	
	Space -> FreeStorageHead = InitialStorage;
	Space -> FreeStorageSlotCount = 111;
	
	InitialRange = & InitialStorage -> Ranges [ 0 ];
	
	InitialRange -> SSIndex = __StorageAndSlotToIndex ( InitialStorage, 0 );
	InitialRange -> Base = NewInitialFreeBase;
	InitialRange -> Length = InitialFreeLength;
	
	InitialRange -> Parent = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	InitialRange -> Left = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	InitialRange -> Right = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	InitialRange -> PrevInSizeClass = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	InitialRange -> NextInSizeClass = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	InitialRange -> Height = 0;
	
	uint32_t SizeClass = __CalculateSizeClass ( InitialFreeLength );
	
	Space -> FreeSizeClassHeads [ SizeClass ] = InitialRange;
	Space -> RootFreeNode = InitialRange;
	
	Space -> FreeNodeCount = 1;
	
	* Error = kCreateAddressSpace_Error_None;
	
};

MM::Paging::AddressSpace :: AddressSpace ()
{
	
	RootFreeNode = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	RootAllocatedNode = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	
	for ( uint32_t I = 0; I < 19; I ++ )
		FreeSizeClassHeads [ I ] = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	
	FreeNodeCount = 0;
	AllocatedNodeCount = 0;
	
	FreeStorageHead = reinterpret_cast <Storage *> ( kStoragePTR_Invalid );
	FullStorageHead = reinterpret_cast <Storage *> ( kStoragePTR_Invalid );
	
	FreeStorageSlotCount = 0;
	AllocatedStorageCount = 0;
	
	FreeStorageHighWatermark = 20;
	
	FreePageCount = 0;
	TotalPageCount = 0;
	
	Kernel = false;
	
};

void MM::Paging::AddressSpace :: Alloc ( uint32_t Length, void ** Base, uint32_t * Error )
{
	
	if ( FreeStorageSlotCount <= 2 )
	{
		
		Storage * NewStorage;
		
		if ( Kernel )
		{
			
			void * NewStoragePhysical;
			
			if ( ! MM::Paging::PFA :: Alloc ( 0x1000, & NewStoragePhysical ) )
			{
				
				* Error = kAlloc_Error_FailedPhysicalAllocation;
				
				return;
				
			}
			
			AddressRange * NewStorageRange = DoAlloc ( 0x1000 );
			
			if ( NewStorageRange == reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
			{
				
				MM::Paging::PFA :: Free ( NewStoragePhysical );
				
				* Error = kAlloc_Error_OutOfKVirtualSpace;
				
				return;
				
			}
			
			MM::Paging::PageTable :: SetKernelMapping ( NewStorageRange -> Length, reinterpret_cast <uint32_t> ( NewStoragePhysical ), MM::Paging::PageTable :: Flags_Present | MM::Paging::PageTable :: Flags_Writeable );
			NewStorage = reinterpret_cast <Storage *> ( NewStorageRange -> Length );
			
		}
		else
		{
			
			NewStorage = reinterpret_cast <Storage *> ( system_func_pmalloc ( 1 ) );
			
			if ( NewStorage == NULL )
			{
				
				* Error = kAlloc_Error_FailedPMalloc;
				
				return;
				
			}
			
		}
		
		NewStorage -> Bitmap [ 0 ] = 0x00000001;
		NewStorage -> Bitmap [ 1 ] = 0x00000000;
		NewStorage -> Bitmap [ 2 ] = 0x00000000;
		NewStorage -> Bitmap [ 3 ] = 0x00000000;
		NewStorage -> FreeCount = 112;
		
		FreeStorageHead -> Previous = NewStorage;
		NewStorage -> Next = FreeStorageHead;
		NewStorage -> Previous = reinterpret_cast <Storage *> ( kStoragePTR_Invalid );
		FreeStorageHead = NewStorage;
		
		FreeStorageSlotCount += 112;
		
	}
	
	AddressRange * AllocationRange = DoAlloc ( Length );
	
	if ( AllocationRange == reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
	{
		
		* Error = kAlloc_Error_OutOfVirtualSpace;
		
		return;
		
	}
	
	* Base = reinterpret_cast <void *> ( AllocationRange -> Base );
	* Error = kAlloc_Error_None;
	
};

void MM::Paging::AddressSpace :: Free ( void * Base, uint32_t * Error )
{
	
	AddressRange * AllocRange = FindAllocatedNode ( reinterpret_cast <uint32_t> ( Base ) );
	
	if ( AllocRange == reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
	{
		
		* Error = kError_Free_NotAllocated;
		
		return;
		
	}
	
	RemoveAllocatedNode ( AllocRange );
	
	FreePageCount += AllocRange -> Length >> 12;
	
	bool Combine = true;
	
	while ( Combine )
	{
		
		Combine = false;
		
		AddressRange * Lower = FindClosestSmallerFreeNode ( AllocRange );
		AddressRange * Higher = FindFreeNode ( AllocRange -> Length + AllocRange -> Base );
		
		if ( Lower != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
		{
			
			if ( ( Lower != AllocRange ) && ( ( Lower -> Base + Lower -> Length ) == AllocRange -> Base ) )
			{
				
				Lower -> Length += AllocRange -> Length;
				FreeOldRange ( AllocRange );
				AllocRange = Lower;
				
				Combine = true;
				
			}
			
		}
		
		if ( Higher != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
		{
			
			RemoveFreeNode ( Higher );
			AllocRange -> Length += Higher -> Length;
			FreeOldRange ( Higher );
			
			Combine = true;
			
		}
		
	}
	
	InsertFreeNode ( AllocRange );
	
	* Error = kFree_Error_None;
	
};

void MM::Paging::AddressSpace :: GetAllocationSize ( void * Base, uint32_t * RecordedSize, uint32_t * Error )
{
	
	AddressRange * AllocRange = FindAllocatedNode ( reinterpret_cast <uint32_t> ( Base ) );
	
	if ( AllocRange == reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
	{
		
		* Error = kGetAllocationSize_Error_NotAllocated;
		
		return;
		
	}
	
	* RecordedSize = AllocRange -> Length;
	* Error = kGetAllocationSize_Error_None;
	
};

uint32_t MM::Paging::AddressSpace :: GetFreePages ()
{
	
	return FreePageCount;
	
};

uint32_t MM::Paging::AddressSpace :: GetTotalPages ()
{
	
	return TotalPageCount;
	
};

MM::Paging::AddressSpace :: AddressRange * MM::Paging::AddressSpace :: FindClosestSmallerFreeNode ( AddressRange * Node )
{
	
	AddressRange * SearchNode = RootFreeNode;
	
	if ( SearchNode == reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
		return SearchNode;
	
	while ( true )
	{
		
		if ( SearchNode -> Base >= Node -> Base )
		{
			
			if ( SearchNode -> Left == reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
				return SearchNode;
			
			if ( SearchNode -> Left -> Base == Node -> Base )
				return SearchNode;
			
			SearchNode = SearchNode -> Left;
			
		}
		else
		{
			
			if ( SearchNode -> Right == reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
				return SearchNode;
			
			SearchNode = SearchNode -> Right;
			
		}
		
	};
	
};

MM::Paging::AddressSpace :: AddressRange * MM::Paging::AddressSpace :: DoAlloc ( uint32_t RequestedSize )
{
	
	RequestedSize += 0xFFF;
	RequestedSize &= ~ 0xFFF;
	
	uint32_t SizeClass = __CalculateSizeClass ( RequestedSize );
	AddressRange * BorrowRange = FindBest ( SizeClass );
	
	if ( BorrowRange == reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
		return BorrowRange;
	
	if ( BorrowRange -> Length == RequestedSize )
	{
		
		RemoveFreeNode ( BorrowRange );
		InsertAllocatedNode ( BorrowRange );
		
		FreePageCount -= RequestedSize >> 12;
		
		return BorrowRange;
		
	}
	
	SizeClass = __CalculateSizeClass ( BorrowRange -> Length );
	
	AddressRange * AllocRange = GetNewRange ();
	
	BorrowRange -> Length -= RequestedSize;
	AllocRange -> Base = BorrowRange -> Base + BorrowRange -> Length;
	AllocRange -> Length = RequestedSize;
	
	uint32_t NewFreeSizeClass = __CalculateSizeClass ( BorrowRange -> Length );
	
	if ( NewFreeSizeClass != SizeClass )
	{
		
		FreeSizeClassHeads [ SizeClass ] = BorrowRange -> NextInSizeClass;
		
		if ( BorrowRange -> NextInSizeClass != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
			FreeSizeClassHeads [ SizeClass ] -> PrevInSizeClass = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
		
		if ( FreeSizeClassHeads [ NewFreeSizeClass ] != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
			FreeSizeClassHeads [ NewFreeSizeClass ] -> PrevInSizeClass = BorrowRange;
		
		BorrowRange -> NextInSizeClass = FreeSizeClassHeads [ NewFreeSizeClass ];
		FreeSizeClassHeads [ NewFreeSizeClass ] = BorrowRange;
		
	}
	
	AllocRange -> Parent = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	AllocRange -> Left = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	AllocRange -> Right = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	AllocRange -> NextInSizeClass = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	AllocRange -> PrevInSizeClass = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	
	InsertAllocatedNode ( AllocRange );
	
	FreePageCount -= RequestedSize >> 12;
	
	return AllocRange;
	
};

MM::Paging::AddressSpace :: AddressRange * MM::Paging::AddressSpace :: GetNewRange ()
{
	
	AddressRange * NewRange;
	
	Storage * AllocStorage = FreeStorageHead;
	uint32_t Slot = __GetFirstFreeStorageSlot ( AllocStorage );
	
	AllocStorage -> FreeCount --;
	
	FreeStorageSlotCount --;
	AllocatedStorageCount ++;
	
	if ( AllocStorage -> FreeCount == 0 )
	{
		
		FreeStorageHead = AllocStorage -> Next;
		FreeStorageHead -> Previous = reinterpret_cast <Storage *> ( kStoragePTR_Invalid );
		
		AllocStorage -> Next = FullStorageHead;
		FullStorageHead -> Previous = AllocStorage;
		FullStorageHead = AllocStorage;
		
	}
	
	__MarkStorageSlotUsed ( AllocStorage, Slot );
	
	NewRange = & AllocStorage -> Ranges [ Slot ];
	NewRange -> SSIndex = __StorageAndSlotToIndex ( AllocStorage, Slot );
	
	return NewRange;
	
};

void MM::Paging::AddressSpace :: FreeOldRange ( AddressRange * Range )
{
	
	Storage * ContainerStorage = __StorageFromIndex ( Range -> SSIndex );
	uint32_t Slot = __SlotFromIndex ( Range -> SSIndex );
	
	__MarkStorageSlotClear ( ContainerStorage, Slot );
	
	ContainerStorage -> FreeCount ++;
	
	if ( ContainerStorage -> FreeCount == 1 )
	{
		
		if ( ContainerStorage == FullStorageHead )
		{
			
			FullStorageHead = ContainerStorage -> Next;
			
			if ( FullStorageHead != reinterpret_cast <Storage *> ( kStoragePTR_Invalid ) )
				FullStorageHead -> Previous = reinterpret_cast <Storage *> ( kStoragePTR_Invalid );
			
		}
		else
		{
			
			if ( ContainerStorage -> Next != reinterpret_cast <Storage *> ( kStoragePTR_Invalid ) )
				ContainerStorage -> Next -> Previous = ContainerStorage -> Previous;
			
			ContainerStorage -> Previous -> Next = ContainerStorage -> Next;
			
		}
		
		if ( FreeStorageHead != reinterpret_cast <Storage *> ( kStoragePTR_Invalid ) )
			FreeStorageHead -> Previous = ContainerStorage;
		
		ContainerStorage -> Next = FreeStorageHead;	
		FreeStorageHead = ContainerStorage;
		
	}
	else if ( ( ContainerStorage -> FreeCount == 112 ) && ( FreeStorageHighWatermark < ( FreeStorageSlotCount * 4096 ) / 112 ) )
	{
		
		FreeStorageSlotCount -= 112;
		
		if ( ContainerStorage == FreeStorageHead )
		{
			
			FreeStorageHead = ContainerStorage -> Next;
			
			if ( FreeStorageHead != reinterpret_cast <Storage *> ( kStoragePTR_Invalid ) )
				FreeStorageHead -> Previous = reinterpret_cast <Storage *> ( kStoragePTR_Invalid );
			
		}
		else
		{
			
			if ( ContainerStorage -> Next != reinterpret_cast <Storage *> ( kStoragePTR_Invalid ) )
				ContainerStorage -> Next -> Previous = ContainerStorage -> Previous;
			
			ContainerStorage -> Previous -> Next = ContainerStorage -> Next;
			
		}
		
		if ( Kernel )
		{
			
			// NOTE: THIS IS A BAD WAY TO HANDLE ERROR CASES
			// At worst, this will just leak space from the
			// virtual address space, but that's not a good thing.
			uint32_t DummyError;
			Free ( reinterpret_cast <void *> ( ContainerStorage ), & DummyError );
			
			MM::Paging::PFA :: Free ( reinterpret_cast <void *> ( MM::Paging::PageTable :: KernelVirtualToPhysical ( reinterpret_cast <uint32_t> ( ContainerStorage ) ) ) );
			MM::Paging::PageTable :: ClearKernelMapping ( reinterpret_cast <uint32_t> ( ContainerStorage ) );
			
			return;
			
		}
		else
		{
			
			system_func_pfree ( reinterpret_cast <void *> ( ContainerStorage ) );
			
			return;
			
		}
		
	}
	
	FreeStorageSlotCount ++;
	AllocatedStorageCount --;
	
};

void MM::Paging::AddressSpace :: InsertFreeNode ( AddressRange * Node )
{
	
	uint32_t SizeClass = __CalculateSizeClass ( Node -> Length );
	
	if ( FreeSizeClassHeads [ SizeClass ] != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
		FreeSizeClassHeads [ SizeClass ] -> PrevInSizeClass = Node;
	
	Node -> NextInSizeClass = FreeSizeClassHeads [ SizeClass ];
	FreeSizeClassHeads [ SizeClass ] = Node;
	
	Node -> Height = 0;
	
	if ( RootFreeNode == reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
	{
		
		RootFreeNode = Node;
		FreeNodeCount ++;
		
		return;
		
	}
	
	AddressRange * Current = RootFreeNode;
	
	while ( true )
	{
		
		if ( Node -> Base > Current -> Base )
		{
			
			if ( Current -> Right == reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
			{
				
				Node -> Parent = Current;
				Current -> Right = Node;
				
				break;
				
			}
			
			Current = Current -> Right;
			
		}
		else
		{
			
			if ( Current -> Left == reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
			{
				
				Node -> Parent = Current;
				Current -> Left = Node;
				
				break;
				
			}
			
			Current = Current -> Left;
			
			
		}
		
	}
	
	Current = Node;
	
	do
	{
		
		Current = Current -> Parent;
		__CalcHeight ( Current );
		
		if ( Current == RootFreeNode )
		{
			
			RootFreeNode = Balance ( Current );
			
			FreeNodeCount ++;
			
			break;
			
		}
		
		Current = Balance ( Current );
		
	} 
	while ( Current -> Parent != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) );
	
};

void MM::Paging::AddressSpace :: InsertAllocatedNode ( AddressRange * Node )
{
	
	Node -> Height = 0;
	
	if ( RootAllocatedNode == reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
	{
		
		RootAllocatedNode = Node;
		AllocatedNodeCount ++;
		
		return;
		
	}
	
	AddressRange * Current = RootAllocatedNode;
	
	while ( true )
	{
		
		if ( Node -> Base > Current -> Base )
		{
			
			if ( Current -> Right == reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
			{
				
				Node -> Parent = Current;
				Current -> Right = Node;
				
				break;
				
			}
			
			Current = Current -> Right;
			
		}
		else
		{
			
			if ( Current -> Left == reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
			{
				
				Node -> Parent = Current;
				Current -> Left = Node;
				
				break;
				
			}
			
			Current = Current -> Left;
			
			
		}
		
	}
	
	Current = Node;
	
	do
	{
		
		Current = Current -> Parent;
		__CalcHeight ( Current );
		
		if ( Current == RootAllocatedNode )
		{
			
			RootAllocatedNode = Balance ( Current );
			
			AllocatedNodeCount ++;
			
			return;
			
		}
		
		Current = Balance ( Current );
		
	} 
	while ( Current -> Parent != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) );
	
};

void MM::Paging::AddressSpace :: RemoveFreeNode ( AddressRange * Node )
{
	
	AddressRange * UnbalancedNode;
	AddressRange * SuccessorNode;
	
	FreeNodeCount --;
	
	uint32_t SizeClass = __CalculateSizeClass ( Node -> Length );
	
	if ( Node -> PrevInSizeClass != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
		Node -> PrevInSizeClass -> NextInSizeClass = Node -> NextInSizeClass;
	else
		FreeSizeClassHeads [ SizeClass ] = Node -> NextInSizeClass;
	
	if ( Node -> NextInSizeClass != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
		Node -> NextInSizeClass -> PrevInSizeClass = Node -> PrevInSizeClass;
	
	if ( Node == RootFreeNode )
	{
		
		if ( Node -> Left != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
		{
			
			if ( Node -> Right != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
			{
				
				SuccessorNode = __LeftMostNode ( Node -> Right );
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
				
				if ( UnbalancedNode == RootFreeNode )
				{
					
					UnbalancedNode = SuccessorNode;
					RootFreeNode -> Right = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
					
				}
				
				RootFreeNode = SuccessorNode;
				SuccessorNode -> Parent = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				
				SuccessorNode -> Left = Node -> Left;
				Node -> Left -> Parent = SuccessorNode;
				
				SuccessorNode -> Right = Node -> Right;
				if ( Node -> Right != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
					Node -> Right -> Parent = SuccessorNode;
				
			}
			else
			{
				
				RootFreeNode = Node -> Left;
				Node -> Left = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				
				return;
				
			}
			
		}
		else
		{
			
			if ( Node -> Right != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
			{
				
				RootFreeNode = Node -> Right;
				Node -> Right = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				
				return;
				
			}
			else
			{
				
				RootFreeNode = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				
				FreeNodeCount = 0;
				
				return;
				
			}
			
		}
		
	}
	else
	{
		
		if ( Node -> Left != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
		{
			
			if ( Node -> Right != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
			{
				
				SuccessorNode = __LeftMostNode ( Node -> Right );
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
				
				if ( UnbalancedNode == Node )
				{
					
					UnbalancedNode = SuccessorNode;
					Node -> Right = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
					
				}
				
				if ( Node -> Parent -> Left == Node )
					Node -> Parent -> Left = SuccessorNode;
				else
					Node -> Parent -> Right = SuccessorNode;
				
				SuccessorNode -> Parent = Node -> Parent;
				
				SuccessorNode -> Right = Node -> Right;
				if ( SuccessorNode -> Right != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
					SuccessorNode -> Right -> Parent = SuccessorNode;
				
				SuccessorNode -> Left = Node -> Left;
				SuccessorNode -> Left -> Parent = SuccessorNode;
				
				Node -> Parent = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				Node -> Left = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				Node -> Right = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				
			}
			else
			{
				
				if ( Node -> Parent -> Right == Node )
					Node -> Parent -> Right = Node -> Left;
				else
					Node -> Parent -> Left = Node -> Left;
				
				UnbalancedNode = Node -> Left -> Parent = Node -> Parent;
				Node -> Parent = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				Node -> Left = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
			}
			
		}
		else
		{
			
			if ( Node -> Right != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
			{
				
				if ( Node -> Parent -> Right == Node )
					Node -> Parent -> Right = Node -> Right;
				else
					Node -> Parent -> Left = Node -> Right;
				
				UnbalancedNode = Node -> Right -> Parent = Node -> Parent;
				Node -> Parent = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				Node -> Right = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				
			}
			else
			{
				
				if ( Node -> Parent -> Right == Node )
					Node -> Parent -> Right = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				else
					Node -> Parent -> Left = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				
				UnbalancedNode = Node -> Parent;
				Node -> Parent = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				
			}
			
		}
		
	}
	
	FreeNodeCount --;
	
	while ( true )
	{
		
		__CalcHeight ( UnbalancedNode );
		
		if ( UnbalancedNode == RootFreeNode )
		{
			
			RootFreeNode = Balance ( UnbalancedNode );
			
			return;
			
		}
		
		UnbalancedNode = Balance ( UnbalancedNode );
		UnbalancedNode = UnbalancedNode -> Parent;
		
	}
	
};

void MM::Paging::AddressSpace :: RemoveAllocatedNode ( AddressRange * Node )
{
	
	AddressRange * UnbalancedNode;
	AddressRange * SuccessorNode;
	
	AllocatedNodeCount --;
	
	if ( Node == RootAllocatedNode )
	{
		
		if ( Node -> Left != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
		{
			
			if ( Node -> Right != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
			{
				
				SuccessorNode = __LeftMostNode ( Node -> Right );
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
				
				if ( UnbalancedNode == RootAllocatedNode )
				{
					
					UnbalancedNode = SuccessorNode;
					RootAllocatedNode -> Right = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
					
				}
				
				RootAllocatedNode = SuccessorNode;
				SuccessorNode -> Parent = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				
				SuccessorNode -> Left = Node -> Left;
				Node -> Left -> Parent = SuccessorNode;
				
				SuccessorNode -> Right = Node -> Right;
				if ( Node -> Right != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
					Node -> Right -> Parent = SuccessorNode;
				
				Node -> Left = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				Node -> Right = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				Node -> Parent = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				
			}
			else
			{
				
				RootAllocatedNode = Node -> Left;
				Node -> Left = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				
				return;
				
			}
			
		}
		else
		{
			
			if ( Node -> Right != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
			{
				
				RootAllocatedNode = Node -> Right;
				Node -> Right = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				
				return;
				
			}
			else
			{
				
				RootAllocatedNode = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				
				AllocatedNodeCount = 0;
				
				return;
				
			}
			
		}
		
	}
	else
	{
		
		if ( Node -> Left != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
		{
			
			if ( Node -> Right != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
			{
				
				SuccessorNode = __LeftMostNode ( Node -> Right );
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
				
				if ( UnbalancedNode == Node )
				{
					
					UnbalancedNode = SuccessorNode;
					Node -> Right = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
					
				}
				
				if ( Node -> Parent -> Left == Node )
					Node -> Parent -> Left = SuccessorNode;
				else
					Node -> Parent -> Right = SuccessorNode;
				
				SuccessorNode -> Parent = Node -> Parent;
				
				SuccessorNode -> Right = Node -> Right;
				if ( SuccessorNode -> Right != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
					SuccessorNode -> Right -> Parent = SuccessorNode;
				
				SuccessorNode -> Left = Node -> Left;
				SuccessorNode -> Left -> Parent = SuccessorNode;
				
				Node -> Parent = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				Node -> Left = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				Node -> Right = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				
			}
			else
			{
				
				if ( Node -> Parent -> Right == Node )
					Node -> Parent -> Right = Node -> Left;
				else
					Node -> Parent -> Left = Node -> Left;
				
				UnbalancedNode = Node -> Left -> Parent = Node -> Parent;
				
				Node -> Parent = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				Node -> Left = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
			}
			
		}
		else
		{
			
			if ( Node -> Right != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
			{
				
				if ( Node -> Parent -> Right == Node )
					Node -> Parent -> Right = Node -> Right;
				else
					Node -> Parent -> Left = Node -> Right;
				
				UnbalancedNode = Node -> Right -> Parent = Node -> Parent;
				Node -> Parent = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				Node -> Right = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				
			}
			else
			{
				
				if ( Node -> Parent -> Right == Node )
					Node -> Parent -> Right = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				else
					Node -> Parent -> Left = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				
				UnbalancedNode = Node -> Parent;
				Node -> Parent = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
				
			}
			
		}
		
	}
	
	while ( true )
	{
		
		__CalcHeight ( UnbalancedNode );
		
		if ( UnbalancedNode == RootAllocatedNode )
		{
			
			RootAllocatedNode = Balance ( UnbalancedNode );
			
			return;
			
		}
		
		UnbalancedNode = Balance ( UnbalancedNode );
		UnbalancedNode = UnbalancedNode -> Parent;
		
	}
	
};

MM::Paging::AddressSpace :: AddressRange * MM::Paging::AddressSpace :: Balance ( AddressRange * Root )
{
	
	int32_t Balance = 0;
	int32_t SubBalance = 0;
	
	Balance += ( Root -> Left != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) ) ? Root -> Left -> Height : - 1;
	Balance -= ( Root -> Right != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) ) ? Root -> Right -> Height : - 1;
	
	if ( Balance > 1 )
	{
		
		SubBalance += ( Root -> Left -> Left != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) ) ? Root -> Left -> Left -> Height : - 1;
		SubBalance -= ( Root -> Left -> Right != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) ) ? Root -> Left -> Right -> Height : - 1;
		
		if ( SubBalance > 0 )
		{
			
			Root = __RotateRight ( Root );
			
		}
		else
		{
			
			__RotateLeft ( Root -> Left );
			Root = __RotateRight ( Root );
			
		}
		
	}
	else if ( Balance < - 1 )
	{
		
		SubBalance -= ( Root -> Right -> Left != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) ) ? Root -> Right -> Left -> Height : - 1;
		SubBalance += ( Root -> Right -> Right != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) ) ? Root -> Right -> Right -> Height : - 1;
		
		if ( SubBalance > 0 )
		{
			
			Root = __RotateLeft ( Root );
			
		}
		else
		{
			
			__RotateRight ( Root -> Right );
			Root = __RotateLeft ( Root );
			
		}
		
	}
	
	return Root;
	
};

MM::Paging::AddressSpace :: AddressRange * MM::Paging::AddressSpace :: FindBest ( uint32_t MinimumSizeClass )
{
	
	while ( MinimumSizeClass <= 19 )
	{
		
		if ( FreeSizeClassHeads [ MinimumSizeClass ] != reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
			return FreeSizeClassHeads [ MinimumSizeClass ];
		
		MinimumSizeClass ++;
		
	}
	
	return reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	
};

MM::Paging::AddressSpace :: AddressRange * MM::Paging::AddressSpace :: FindFreeNode ( uint32_t Address )
{
	
	AddressRange * Node = RootFreeNode;
	
	while ( true )
	{
		
		if ( Node == reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
			return reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
		
		if ( Node -> Base == Address )
			return Node;
		
		if ( Node -> Base > Address )
			Node = Node -> Left;
		else if ( Node -> Base < Address )
			Node = Node -> Right;
		
	}
	
};

MM::Paging::AddressSpace :: AddressRange * MM::Paging::AddressSpace :: FindAllocatedNode ( uint32_t Address )
{
	
	AddressRange * Node = RootAllocatedNode;
	
	while ( true )
	{
		
		if ( Node == reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
			return reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
		
		if ( Node -> Base == Address )
			return Node;
		
		if ( Node -> Base > Address )
			Node = Node -> Left;
		else if ( Node -> Base < Address )
			Node = Node -> Right;
		
	}
	
};

void MM::Paging::AddressSpace :: DebugPrint ()
{

	system_func_kprintf ( "FREE TREE: " );
	__TreePrint ( RootFreeNode );
	system_func_kprintf ( "\nUSED TREE: " );	
	__TreePrint ( RootAllocatedNode );
	system_func_kprintf ( "\n" );
	
};

void * MM::Paging::AddressSpace :: operator new ( size_t, void * Address )
{
	
	return Address;
	
};

MM::Paging::AddressSpace :: ~AddressSpace ()
{
	
	while ( FreeStorageHead != reinterpret_cast <Storage *> ( kStoragePTR_Invalid ) )
	{
		
		Storage * PendingFreeStorage = FreeStorageHead;
		FreeStorageHead = PendingFreeStorage -> Next;
		
		system_func_pfree ( reinterpret_cast <void *> ( PendingFreeStorage ) );
		
	}
	
	while ( FreeStorageHead != reinterpret_cast <Storage *> ( kStoragePTR_Invalid ) )
	{
		
		Storage * PendingFreeStorage = FreeStorageHead;
		FreeStorageHead = PendingFreeStorage -> Next;
		
		system_func_pfree ( reinterpret_cast <void *> ( PendingFreeStorage ) );
		
	}
	
	
};

void MM::Paging::AddressSpace :: AddFreeRange ( uint32_t Base, uint32_t Length, uint32_t * Error )
{
	
	uint32_t NewBase = 0xFFF + Base;
	NewBase &= ~ 0xFFF;
	Length -= NewBase - Base;
	Length &= ~ 0xFFF;
	
	if ( Length == 0 )
		return;
	
	if ( FreeStorageSlotCount <= 2 )
	{
		
		Storage * NewStorage;
		
		if ( Kernel )
		{
			
			void * NewStoragePhysical;
			
			if ( ! MM::Paging::PFA :: Alloc ( 0x1000, & NewStoragePhysical ) )
			{
				
				* Error = kAlloc_Error_FailedPhysicalAllocation;
				
				return;
				
			}
			
			AddressRange * NewStorageRange = DoAlloc ( 0x1000 );
			
			if ( NewStorageRange == reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid ) )
			{
				
				if ( Length > 0x1000 )
				{
					
					Length -= 0x1000;
					NewStorageRange = reinterpret_cast <AddressRange *> ( NewBase + Length );
					
				}
				else
				{
					
					MM::Paging::PFA :: Free ( NewStoragePhysical );
					
					* Error = kAddFreeRange_Error_OutOfKVirtualSpace;
					
					return;
					
				}
				
			}
			
			MM::Paging::PageTable :: SetKernelMapping ( NewStorageRange -> Length, reinterpret_cast <uint32_t> ( NewStoragePhysical ), MM::Paging::PageTable :: Flags_Present | MM::Paging::PageTable :: Flags_Writeable );
			NewStorage = reinterpret_cast <Storage *> ( NewStorageRange -> Length );
			
		}
		else
		{
			
			NewStorage = reinterpret_cast <Storage *> ( system_func_pmalloc ( 1 ) );
			
		}
		
		NewStorage -> Bitmap [ 0 ] = 0x00000001;
		NewStorage -> Bitmap [ 1 ] = 0x00000000;
		NewStorage -> Bitmap [ 2 ] = 0x00000000;
		NewStorage -> Bitmap [ 3 ] = 0x00000000;
		NewStorage -> FreeCount = 112;
		
		FreeStorageHead -> Previous = NewStorage;
		NewStorage -> Next = FreeStorageHead;
		NewStorage -> Previous = reinterpret_cast <Storage *> ( kStoragePTR_Invalid );
		FreeStorageHead = NewStorage;
		
		FreeStorageSlotCount += 112;
		
	}
	
	AddressRange * NewRange = GetNewRange ();
	
	NewRange -> Parent = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	NewRange -> Left = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	NewRange -> Right = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	NewRange -> PrevInSizeClass = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	NewRange -> NextInSizeClass = reinterpret_cast <AddressRange *> ( kAddressRangePTR_Invalid );
	NewRange -> Base = NewBase;
	NewRange -> Length = Length;
	
	TotalPageCount += Length >> 12;
	FreePageCount += Length >> 12;
	
	InsertFreeNode ( NewRange );
	
	* Error = kAddFreeRange_Error_None;
	
};
