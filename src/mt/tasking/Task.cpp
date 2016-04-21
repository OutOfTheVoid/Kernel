#include <mt/tasking/Task.h>
#include <mt/tasking/Scheduler.h>

#include <mt/process/UserProcess.h>

#include <cpputil/Linkage.h>

#include <mm/KMalloc.h>
#include <mm/PMalloc.h>

#include <mm/paging/PageTable.h>
#include <mm/paging/AddressSpace.h>
#include <mm/paging/Invalidation.h>
#include <mm/paging/PFA.h>

#include <util/string/String.h>

#include <hw/cpu/Processor.h>

#include <system/permissions/UserList.h>

ASM_LINKAGE void interrupt_ISRCommonHandlerInjectionReturn ();

MT::Tasking :: Task * MT::Tasking::Task :: CreateKernelTask ( const char * Name, void * Entry, uint32_t StackSize, uint32_t MaxPriority, uint32_t MinPriority )
{
	
	Task * New = new Task ();
	
	if ( New == NULL )
		return NULL;
	
	strcpy ( New -> Name, Name );
	
	New -> ID = Scheduler :: GetNewTaskID ();
	
	New -> Flags = kFlag_Kernel;
	New -> State = kState_Runnable;
	New -> User = System::Permissions::UserList :: kUserID_System;
	New -> Privelege = kPrivelege_Exec | kPrivelege_IO;
	New -> Priority = MaxPriority;
	New -> MaxPriority = MaxPriority;
	New -> MinPriority = MinPriority;
	
	New -> MemoryMapping = NULL;
	New -> MemorySpace = NULL;
	
	New -> Process = NULL;
	
	StackSize += 0xFFF;
	StackSize &= ~ 0xFFF;
	
	void * Stack = mm_pmalloc ( ( StackSize >> 12 ) + 1 );
	
	if ( Stack == NULL )
	{
		
		mm_kfree ( reinterpret_cast <void *> ( New ) );
		
		return NULL;
		
	}
	
	New -> KStackBottom = Stack;
	New -> KStack = reinterpret_cast <void *> ( reinterpret_cast <uint32_t> ( Stack ) + StackSize );
	New -> KSS = 0x10;
	
	New -> UStackBottom = NULL;
	New -> UStack = NULL;
	New -> USS = 0x00;
	
	StackPush ( & New -> KStack, 0x202 ); // EFlags
	StackPush ( & New -> KStack, 0x08 ); // CS
	StackPush ( & New -> KStack, reinterpret_cast <uint32_t> ( Entry ) ); // EIP
	
	StackPush ( & New -> KStack, 0 ); // Error code
	StackPush ( & New -> KStack, 0x20 ); // Interrupt number
	
	uint32_t OESP = reinterpret_cast <uint32_t> ( New -> KStack );
	
	StackPush ( & New -> KStack, 0 ); // EAX
	StackPush ( & New -> KStack, 0 ); // ECX
	StackPush ( & New -> KStack, 0 ); // EDX
	StackPush ( & New -> KStack, 0 ); // EBX
	StackPush ( & New -> KStack, OESP ); // ESP
	StackPush ( & New -> KStack, 0 ); // EBP
	StackPush ( & New -> KStack, 0 ); // ESI
	StackPush ( & New -> KStack, 0 ); // EDI
	
	StackPush ( & New -> KStack, 0x10 ); // DS
	
	StackPush ( & New -> KStack, reinterpret_cast <uint32_t> ( & interrupt_ISRCommonHandlerInjectionReturn ) );
	
	StackPush ( & New -> KStack, 0 ); // EBP
	StackPush ( & New -> KStack, 0 ); // EBX
	StackPush ( & New -> KStack, 0 ); // ESI
	StackPush ( & New -> KStack, 0 ); // EDI
	
	return New;
	
};

MT::Tasking :: Task * MT::Tasking::Task :: CreateUserTask ( const char * Name, Process :: UserProcess * ContainerProcess, void * Entry, uint32_t StackSize, uint32_t KStackSize, uint32_t MaxPriority, uint32_t MinPriority )
{
	
	Task * New = new Task ();
	
	if ( New == NULL )
		return NULL;
	
	strcpy ( New -> Name, Name );
	
	New -> Process = ContainerProcess;
	
	New -> ID = Scheduler :: GetNewTaskID ();
	
	New -> Flags = kFlag_User;
	New -> State = kState_Runnable;
	New -> User = 0;
	New -> Privelege = kPrivelege_None;
	New -> Priority = MaxPriority;
	New -> MaxPriority = MaxPriority;
	New -> MinPriority = MinPriority;
	
	New -> MemoryMapping = ContainerProcess -> MemoryMapping;
	New -> MemorySpace = ContainerProcess -> MemorySpace;
	
	StackSize += 0xFFF;
	StackSize &= ~ 0xFFF;
	
	void * Stack;
	void * StackPhys;
	uint32_t AllocError;
	
	ContainerProcess -> MemorySpace -> Alloc ( StackSize, & Stack, & AllocError );
	
	if ( AllocError != MM::Paging::AddressSpace :: kAlloc_Error_None )
	{
		
		delete New;
		
		return NULL;
		
	}
	
	if ( ! MM::Paging::PFA :: Alloc ( StackSize, & StackPhys ) )
	{
		
		delete New;
		
		ContainerProcess -> MemorySpace -> Free ( Stack, & AllocError );
		
		return NULL;
		
	}
	
	ContainerProcess -> MemoryMapping -> SetRegionMapping ( reinterpret_cast <uint32_t> ( Stack ), reinterpret_cast <uint32_t> ( StackPhys ), StackSize, MM::Paging::PageTable :: Flags_User | MM::Paging::PageTable :: Flags_Writeable | MM::Paging::PageTable :: Flags_Present );
	
	if ( Stack == NULL )
	{
		
		mm_kfree ( reinterpret_cast <void *> ( New ) );
		
		return NULL;
		
	}
	
	New -> UStackBottom = Stack;
	New -> UStack = reinterpret_cast <void *> ( reinterpret_cast <uint32_t> ( Stack ) + StackSize );
	New -> USS = 0x20;
	
	KStackSize += 0xFFF;
	KStackSize &= ~ 0xFFF;
	
	void * KStack = mm_pmalloc ( ( KStackSize >> 12 ) + 1 );
	
	if ( KStack == NULL )
	{
		
		mm_pfree ( Stack );
		mm_kfree ( reinterpret_cast <void *> ( New ) );
		
		return NULL;
		
	}
	
	New -> KStackBottom = KStack;
	New -> KStack = reinterpret_cast <void *> ( reinterpret_cast <uint32_t> ( KStack ) + KStackSize );
	New -> KSS = 0x10;
	
	StackPush ( & New -> KStack, 0x20 + 3 ); // SS
	StackPush ( & New -> KStack, reinterpret_cast <uint32_t> ( New -> UStack ) ); // ESP
	StackPush ( & New -> KStack, 0x202 ); // EFlags
	StackPush ( & New -> KStack, 0x18 + 3 ); // CS
	StackPush ( & New -> KStack, reinterpret_cast <uint32_t> ( Entry ) ); // EIP
	
	StackPush ( & New -> KStack, 0 ); // Error code
	StackPush ( & New -> KStack, 0x20 ); // Interrupt number
	
	uint32_t OESP = reinterpret_cast <uint32_t> ( New -> KStack );
	
	StackPush ( & New -> KStack, 0 ); // EAX
	StackPush ( & New -> KStack, 0 ); // ECX
	StackPush ( & New -> KStack, 0 ); // EDX
	StackPush ( & New -> KStack, 0 ); // EBX
	StackPush ( & New -> KStack, OESP ); // ESP
	StackPush ( & New -> KStack, 0 ); // EBP
	StackPush ( & New -> KStack, 0 ); // ESI
	StackPush ( & New -> KStack, 0 ); // EDI
	
	StackPush ( & New -> KStack, 0x20 + 3 ); // DS
	
	StackPush ( & New -> KStack, reinterpret_cast <uint32_t> ( & interrupt_ISRCommonHandlerInjectionReturn ) );
	
	StackPush ( & New -> KStack, 0 ); // EBP
	StackPush ( & New -> KStack, 0 ); // EBX
	StackPush ( & New -> KStack, 0 ); // ESI
	StackPush ( & New -> KStack, 0 ); // EDI
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & ContainerProcess -> ThreadListLock );
	
	New -> ThreadID = ContainerProcess -> Threads -> Length ();
	ContainerProcess -> Threads -> Push ( New );
	
	MT::Synchronization::Spinlock :: Release ( & ContainerProcess -> ThreadListLock );
	
	return New;
	
};

void MT::Tasking::Task :: SyncUserStack ( volatile Task * ToUpdate, Interrupt::InterruptHandlers :: ISRFrame * IFrame )
{
	
	if ( IFrame -> SS == ToUpdate -> USS )
		ToUpdate -> UStack = reinterpret_cast <void *> ( IFrame -> UserESP );
	
};

void MT::Tasking::Task :: DestroyKernelTask ( volatile Task * ToDestroy )
{
	
	if ( ( ToDestroy -> Flags & kFlag_CPUInitStack ) != 0 )
	{
		
		uint32_t I;
		::HW::CPU::Processor :: CPUInfo * Info;
		
		for ( I = 0; I < ::HW::CPU::Processor :: GetProcessorCount (); I ++ )
		{
			
			Info = ::HW::CPU::Processor :: GetProcessorByIndex ( I );
			
			if ( Info -> InitStackBottom == ToDestroy -> KStackBottom )
			{
				
				if ( mm_psize ( ToDestroy -> KStackBottom ) != 0 )
					mm_pfree ( ToDestroy -> KStackBottom );
				else
				{
					
					uint32_t DummyError;
					
					MM::Paging::PageTable :: ClearKernelRegionMapping ( reinterpret_cast <uint32_t> ( Info -> InitStackBottom ), Info -> InitStackLength );
					MM::Paging::Invalidation :: InvalidatePages ( reinterpret_cast <uint32_t> ( Info -> InitStackBottom ), Info -> InitStackLength / 0x1000 );
					
					MM::Paging::PFA :: AddFreeRange ( Info -> InitStackBottom, Info -> InitStackLength );
					
					MM::Paging::AddressSpace :: RetrieveKernelAddressSpace () -> AddFreeRange ( reinterpret_cast <uint32_t> ( Info -> InitStackBottom ), Info -> InitStackLength, & DummyError );
					
				}
				
			}
			
		}
		
	}
	else
		mm_pfree ( ToDestroy -> KStackBottom );
	
	mm_kfree ( const_cast <void *> ( reinterpret_cast <volatile void *> ( ToDestroy ) ) );
	
};

void MT::Tasking::Task :: StackPush ( void ** Stack, uint32_t Value )
{
	
	* Stack = reinterpret_cast <void *> ( reinterpret_cast <uint32_t> ( * Stack ) - 4 );
	* reinterpret_cast <uint32_t *> ( * Stack ) = Value;
	
};
