#include <util/string/string.h>

C_LINKAGE size_t strlen ( const char * Str )
{

	const char * Str2 = Str;

	while ( * ( Str2 ++ ) );

	return Str2 - Str;

};

C_LINKAGE size_t strcmp ( const char * Str1, const char * Str2 )
{

	size_t Res = 0;

	while ( ! ( Res = * reinterpret_cast <const unsigned char *> ( Str1 ) - * reinterpret_cast <const unsigned char *> ( Str2 ) ) && * Str2 )
	{	

		++ Str1;
		++ Str2;

	}

	return Res;
};

C_LINKAGE void atoi ( char * Str, int32_t * Int )
{

	int32_t Value = 0;

	while ( * Str )
	{
		
		Value = ( Value << 3 ) + ( Value << 1 ) + ( * Str ) - '0';

		Str ++;

	}

	* Int = Value;

};

C_LINKAGE const char * itoa ( int32_t Value, char * Str, int32_t Base )
{

	char * Buffer;
	char * SubString;
	char * NumStart;

	if ( Base < 2 || Base > 36 )
	{

		* Str = '\0';

		return Str;
	
	}

	Buffer = SubString = Str;
	
	if ( Value < 0 && Base == 10 )
		* SubString ++ = '-';
	
	NumStart = Buffer;
	
	do
	{
		
		* SubString ++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [ 35 + Value % Base ];
		
		Value /= Base;

	}
	while ( Value );
	
	* SubString -- = '\0';
	
	while ( NumStart < SubString )
	{
		
		char Temp = * NumStart;
		
		* NumStart ++ = * SubString;
		* SubString -- = Temp;
	}

	return Buffer;

};

C_LINKAGE const char * utoa ( uint32_t Value, char * Str, int32_t Base )
{
	
	char * Buffer;
	char * SubString;
	char * NumStart;

	if ( Base < 2 || Base > 36 )
	{

		* Str = '\0';

		return Str;
	
	}

	Buffer = SubString = Str;
	
	NumStart = Buffer;
	
	do
	{
		
		* SubString ++ = "0123456789abcdefghijklmnopqrstuvwxyz" [ Value % Base ];
		
		Value /= Base;

	}
	while ( Value );
	
	* SubString -- = '\0';
	
	while ( NumStart < SubString )
	{
		
		char Temp = * NumStart;
		
		* NumStart ++ = * SubString;
		* SubString -- = Temp;
	}

	return Buffer;
	
};
	
C_LINKAGE void * memcpy ( void * Destination, const void * Source, size_t Count )
{
	
#ifdef __builtin_memcpy
	
	return __builtin_memcpy ( Destination, Source, Count );
	
#else
	
	char * _Destination = reinterpret_cast <char *> ( Destination );
	const char * _Source = reinterpret_cast <const char *> ( Source );
	
	uint32_t I = 0;
	
	while ( ( ( reinterpret_cast <uintptr_t> ( _Destination ) & ~ 7 ) != 0 ) && I < Count )
	{
		
		* _Destination ++ = * _Source ++;
		I ++;
		
	}
	
	Count -= I;
	I = 0;
	
	uint64_t * __Destination = reinterpret_cast <uint64_t *> ( _Destination );
	const uint64_t * __Source = reinterpret_cast <const uint64_t *> ( _Source );
	
	while ( I < ( Count & ~ 7 ) )
	{
		
		* __Destination ++ = * __Source ++;
		I += 8;
		
	}
	
	_Destination = reinterpret_cast <char *> ( __Destination );
	_Source = reinterpret_cast <const char *> ( __Source );
	
	while ( I < Count )
	{
		
		* _Destination ++ = * _Source ++;
		I ++;
		
	}
	
	return Destination;
	
#endif
	
}

