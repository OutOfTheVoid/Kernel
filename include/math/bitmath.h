#ifndef MATH_BITMATH_H
#define MATH_BITMATH_H

#include <stdint.h>

inline uint32_t math_bitmath_clz ( uint32_t In )
{
	
#ifdef __builtin_clz
	return __builtin_clz ( In );
#else
	uint32_t A;
	
	if ( In == 0 )
		return 32;
	for ( A = 0; ( In & 0x80000000 ) == 0; A ++ )
		In <<= 1;
	return A;
#endif
	
};

inline uint32_t math_bitmath_ctz ( uint32_t In )
{
	
	return __builtin_ctz ( In );
	
}

inline uint32_t math_bitmath_nextPowerOfTwo ( uint32_t In )
{
	
	In --;
	
	In |= In >> 1;
	In |= In >> 2;
	In |= In >> 4;
	In |= In >> 8;
	In |= In >> 16;
	
	In ++;
	
	return In;
	
}

#endif
