#include <mt/tasking/Task.h>
#include <mt/tasking/Scheduler.h>

#include <cpputil/Linkage.h>

#include <mm/KMalloc.h>
#include <mm/PMalloc.h>

#include <mm/paging/PageTable.h>
#include <mm/paging/AddressSpace.h>
#include <mm/paging/PFA.h>

#include <util/string/String.h>

#include <hw/cpu/Processor.h>

ASM_LINKAGE void interrupt_ISRCommonHandlerInjectionReturn ();

MT::Tasking::Task :: Task_t * MT::Tasking::Task :: CreateKernelTask ( const char * Name, void * Entry, uint32_t StackSize, uint32_t Priority )
{
	
	Task_t * New = reinterpret_cast <MT::Tasking::Task :: Task_t *> ( mm_kmalloc ( sizeof ( Task :: Task_t ) ) );
	
	if ( New == NULL )
		return NULL;
	
	strcpy ( New -> Name, Name );
	
	New -> ID = Scheduler :: GetNewTaskID ();
	
	New -> Flags = kFlag_Kernel;
	New -> State = kState_Runnable;
	New -> User = 0;
	New -> Privelege = kPrivelege_Exec | kPrivelege_IO;
	New -> Priority = Priority;
	
	New -> MemoryMapping = NULL;
	New -> MemorySpace = NULL;
	
	StackSize += 0xFFF;
	StackSize &= ~ 0xFFF;
	
	void * Stack = mm_pmalloc ( ( StackSize >> 12 ) + 1 );
	
	New -> KStackBottom = Stack;
	
	if ( Stack == NULL )
	{
		
		mm_kfree ( reinterpret_cast <void *> ( New ) );
		
		return NULL;
		
	}
	
	New -> KStack = reinterpret_cast <void *> ( reinterpret_cast <uint32_t> ( Stack ) + StackSize );
	
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

void MT::Tasking::Task :: DestroyKernelTask ( Task_t * ToDestroy )
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
					
					MM::Paging::PFA :: AddFreeRange ( Info -> InitStackBottom, Info -> InitStackLength );
					
					MM::Paging::AddressSpace :: RetrieveKernelAddressSpace () -> AddFreeRange ( reinterpret_cast <uint32_t> ( Info -> InitStackBottom ), Info -> InitStackLength, & DummyError );
					
				}
				
			}
			
		}
		
	}
	else
		mm_pfree ( ToDestroy -> KStackBottom );
	
	mm_kfree ( ToDestroy );
	
};

void MT::Tasking::Task :: StackPush ( void ** Stack, uint32_t Value )
{
	
	* Stack = reinterpret_cast <void *> ( reinterpret_cast <uint32_t> ( * Stack ) - 4 );
	* reinterpret_cast <uint32_t *> ( * Stack ) = Value;
	
};
