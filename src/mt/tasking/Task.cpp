#include <mt/tasking/Task.h>
#include <mt/tasking/Scheduler.h>

#include <cpputil/Linkage.h>

#include <mm/KMalloc.h>
#include <mm/PMalloc.h>

#include <util/string/String.h>

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
	
	void * KStack = mm_pmalloc ( 4 );
	
	if ( KStack == NULL )
	{
		
		mm_kfree ( reinterpret_cast <void *> ( New ) );
		
		return NULL;
		
	}
	
	StackSize += 0xFFF;
	StackSize &= ~ 0xFFF;
	
	void * TStack = mm_pmalloc ( StackSize >> 12 );
	
	if ( TStack == NULL )
	{
		
		mm_pfree ( KStack );
		mm_kfree ( reinterpret_cast <void *> ( New ) );
		
		return NULL;
		
	}
	
	TStack = reinterpret_cast <void *> ( reinterpret_cast <uint32_t> ( TStack ) + StackSize );
	
	New -> KStack = reinterpret_cast <void *> ( reinterpret_cast <uint32_t> ( KStack ) + 0x4000 );
	New -> KSS = 0x10;
	
	StackPush ( & New -> KStack, 0x10 ); // SS
	StackPush ( & New -> KStack, reinterpret_cast <uint32_t> ( TStack ) ); // UserESP
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

void MT::Tasking::Task :: StackPush ( void ** Stack, uint32_t Value )
{
	
	* Stack = reinterpret_cast <void *> ( reinterpret_cast <uint32_t> ( * Stack ) - 4 );
	* reinterpret_cast <uint32_t *> ( * Stack ) = Value;
	
};
