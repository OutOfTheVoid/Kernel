#ifndef UTIL_VECTOR_H
#define UTIL_VECTOR_H

#include <system/func/Panic.h>

#include <mm/KMalloc.h>

template <typename T>
class Vector
{
public:
	
	Vector ()
	{
		
		Elements = reinterpret_cast <T *> ( mm_kmalloc ( sizeof ( T ) * kDataAllocation_DefaultSize ) );
		
		if ( Elements == NULL )
			KPANIC ( "Vector allocation failed!" );
		
		Allocated = kDataAllocation_DefaultSize;
		Used = 0;
		
	};
	
	~Vector ()
	{
		
		mm_kfree ( Elements );
		
	};
	
	inline T & operator[] ( size_t Index )
	{
		
		return Elements [ Index ];
		
	};
	
	inline size_t Length ()
	{
		
		return Used;
		
	};
	
	inline void Push ( T Element )
	{
		
		if ( Used + 1 == Allocated )
			Grow ( 1 );
		
		Elements [ Used ] = Element;
		Used ++;
		
	};
	
	inline T Pop ()
	{
		
		if ( Used == 0 )
			KPANIC ( "Attempt to pop from a vector that had no elements!" );
		
		Used --;
		
		return Elements [ Used ];
		
	};
	
	inline T & Peek ()
	{
		
		return Elements [ Used - 1 ];
		
	};
	
	inline T UnShift ()
	{
		
		if ( Used == 0 )
			KPANIC ( "Attempt to shift from a vector that had no elements!" );
		
		T Temporary = Elements [ 0 ];
		
		Used --;
		
		for ( uint32_t i = 0; i < Used; i ++ )
			Elements [ i ] = Elements [ i + 1 ];
		
		return Temporary;
		
	};
	
	inline void Remove ( uint32_t Start, uint32_t Length )
	{
		
		if ( Start >= Used )
			return;
		
		if ( Start + Length > Used )
			Length = Used - Start;
		
		for ( uint32_t i = Start; i < Used - Length; i ++ )
			Elements [ i ] = Elements [ i + Length ];
		
		Used -= Length;
		
	};
	
	inline void Clear ()
	{
		
		Used = 0;
		
	};
	
	inline int32_t IndexOf ( T Element, uint32_t Start = 0 )
	{
		
		for ( uint32_t i = Start; i < Used; i ++ )
			if ( Elements [ i ] == Element )
				return i;
			
		return - 1;
		
	};
	
private:
	
	static const uint32_t kDataAllocation_Oversized = 0x10000;
	static const uint32_t kDataAllocation_MinGrowthPad = 0x10;
	static const uint32_t kDataAllocation_DefaultSize = 0x10;
	
	void Grow ( size_t Minimum )
	{
		
		size_t GrownSize = Allocated * 2;
		
		void * TempElements = mm_krealloc ( Elements, GrownSize * sizeof ( T ) );
		
		if ( TempElements == NULL )
		{
			
			if ( GrownSize * sizeof ( T ) > kDataAllocation_Oversized )
			{
				
				GrownSize = Allocated + Minimum + kDataAllocation_MinGrowthPad;
				
				TempElements = mm_krealloc ( Elements, GrownSize * sizeof ( T ) );
				
				if ( TempElements == NULL )
				{
					
					KPANIC ( "Vector reallocation failed!" );
					
					return;
					
				}
				
				this -> Elements = reinterpret_cast <T *> ( TempElements );
				this -> Allocated = GrownSize;
				
			}
			
			KPANIC ( "Vector reallocation failed!" );
			
		}
		
		this -> Elements = reinterpret_cast <T *> ( TempElements );
		this -> Allocated = GrownSize;
		
	};
	
	T * Elements;
	
	size_t Allocated;
	size_t Used;
	
};

#endif
